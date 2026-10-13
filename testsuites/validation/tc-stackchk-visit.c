/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsStackchkReqVisit
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <string.h>
#include <rtems/stackchk.h>
#include <rtems/stackchkimpl.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextdata.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsStackchkReqVisit spec:/rtems/stackchk/req/visit
 *
 * @ingroup TestsuitesValidation0
 * @ingroup TestsuitesValidationAcfg1
 *
 * The configuration of the test suite decides whether it enables the stack
 * checker, so the test skips the variants of the other state.
 *
 * The processor of some architectures serves interrupts on the stack of the
 * interrupted thread and never executes on its interrupt stack. The test
 * therefore establishes the execution on the interrupt stack of processor 0
 * itself. The processor uses one word of the interrupt stack first. If this
 * word still holds the fill pattern of the stack checker, the test writes zero
 * to it.
 *
 * The test marks a word of the usable stack as used. It writes zero to this
 * word, so the test knows the deepest use of the stack of a thread. The
 * representative Middle marks the word at the middle and the representative
 * Deepest marks the deepest word of the usable stack. For an interrupt stack,
 * an interrupt may use the stack deeper than the middle. The action restores
 * the marked word, so the later visits of the interrupt stack do not see the
 * mark.
 *
 * The test sets a name of the length of the representative for the test task.
 * It checks this length, if the configured maximum thread name size allows it.
 * It takes the expected name from the name of the thread.
 *
 * The member of the stack pointer in a saved context depends on the
 * architecture and on some architectures also on the address of the context.
 * The test locates the member in the context of the test task.
 *
 * @{
 */

typedef enum {
  RtemsStackchkReqVisit_Pre_Enabled_Yes,
  RtemsStackchkReqVisit_Pre_Enabled_No,
  RtemsStackchkReqVisit_Pre_Enabled_NA
} RtemsStackchkReqVisit_Pre_Enabled;

typedef enum {
  RtemsStackchkReqVisit_Pre_Kind_Thread,
  RtemsStackchkReqVisit_Pre_Kind_ISR,
  RtemsStackchkReqVisit_Pre_Kind_NA
} RtemsStackchkReqVisit_Pre_Kind;

typedef enum {
  RtemsStackchkReqVisit_Pre_HasExecuted_Yes,
  RtemsStackchkReqVisit_Pre_HasExecuted_No,
  RtemsStackchkReqVisit_Pre_HasExecuted_NA
} RtemsStackchkReqVisit_Pre_HasExecuted;

typedef enum {
  RtemsStackchkReqVisit_Pre_StackPointer_Below,
  RtemsStackchkReqVisit_Pre_StackPointer_Inside,
  RtemsStackchkReqVisit_Pre_StackPointer_Above,
  RtemsStackchkReqVisit_Pre_StackPointer_NA
} RtemsStackchkReqVisit_Pre_StackPointer;

typedef enum {
  RtemsStackchkReqVisit_Post_Id_Thread,
  RtemsStackchkReqVisit_Post_Id_Processor,
  RtemsStackchkReqVisit_Post_Id_NA
} RtemsStackchkReqVisit_Post_Id;

typedef enum {
  RtemsStackchkReqVisit_Post_Name_Thread,
  RtemsStackchkReqVisit_Post_Name_Interrupt,
  RtemsStackchkReqVisit_Post_Name_NA
} RtemsStackchkReqVisit_Post_Name;

typedef enum {
  RtemsStackchkReqVisit_Post_Area_Allocated,
  RtemsStackchkReqVisit_Post_Area_NA
} RtemsStackchkReqVisit_Post_Area;

typedef enum {
  RtemsStackchkReqVisit_Post_StackPointer_Context,
  RtemsStackchkReqVisit_Post_StackPointer_Null,
  RtemsStackchkReqVisit_Post_StackPointer_NA
} RtemsStackchkReqVisit_Post_StackPointer;

typedef enum {
  RtemsStackchkReqVisit_Post_Used_Zero,
  RtemsStackchkReqVisit_Post_Used_Deepest,
  RtemsStackchkReqVisit_Post_Used_Unknown,
  RtemsStackchkReqVisit_Post_Used_NA
} RtemsStackchkReqVisit_Post_Used;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Enabled_NA : 1;
  uint16_t Pre_Kind_NA : 1;
  uint16_t Pre_HasExecuted_NA : 1;
  uint16_t Pre_StackPointer_NA : 1;
  uint16_t Post_Id : 2;
  uint16_t Post_Name : 2;
  uint16_t Post_Area : 1;
  uint16_t Post_StackPointer : 2;
  uint16_t Post_Used : 2;
} RtemsStackchkReqVisit_Entry;

typedef enum { KIND_THREAD, KIND_ISR } Kind;

typedef enum { POSITION_BELOW, POSITION_INSIDE, POSITION_ABOVE } Position;

typedef enum {
  POINT_FAR,
  POINT_EDGE,
  POINT_FIRST,
  POINT_MIDDLE,
  POINT_LAST
} Point;

/*
 * This is the size of the name buffer of the stack checker including the
 * terminating null character.
 */
#define NAME_SIZE 22

/**
 * @brief Test context for spec:/rtems/stackchk/req/visit test case.
 */
typedef struct {
  /**
   * @brief This member specifies the kind of the visited stack.
   */
  Kind kind;

  /**
   * @brief This member specifies the length of the name of the test task.
   */
  size_t name_length;

  /**
   * @brief This member is true, if the test task shall execute.
   */
  bool execute;

  /**
   * @brief This member specifies the position of the stack pointer of the
   *   saved context of the test task.
   */
  Position position;

  /**
   * @brief This member is true, if the test task executed.
   */
  volatile bool executed;

  /**
   * @brief This member contains the identifier of the test task.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the begin address of the allocated stack area
   *   of the visited stack.
   */
  uintptr_t area_begin;

  /**
   * @brief This member contains the end address of the allocated stack area of
   *   the visited stack.
   */
  uintptr_t area_end;

  /**
   * @brief This member contains the count of visits of the selected stack.
   */
  uint32_t visits;

  /**
   * @brief This member contains the stack information of the selected stack.
   */
  rtems_stack_checker_info info;

  /**
   * @brief This member contains a copy of the name in the stack information.
   */
  char name[ 2 * NAME_SIZE ];

  /**
   * @brief This member contains the expected name in the stack information.
   */
  char expected_name[ 2 * NAME_SIZE ];

  /**
   * @brief This member is true, if the action shall mark the deepest word of
   *   the stack as used, otherwise the word at the middle.
   */
  bool use_deepest;

  /**
   * @brief This member specifies the value of the stack pointer within its
   *   position.
   */
  Point point;

  /**
   * @brief This member references the word which the action marked as used, if
   *   it is not NULL.
   */
  uint32_t *used_word;

  /**
   * @brief This member contains the value of the marked word before the action
   *   marked it.
   */
  uint32_t used_word_value;

  /**
   * @brief This member contains the distance in bytes from the begin of the
   *   used stack area to the word which the action marks as used.
   */
  uintptr_t used_depth;

  /**
   * @brief This member contains the stack pointer of the saved context of the
   *   test task during the call.
   */
  uintptr_t stack_pointer;

  /**
   * @brief This member contains the stack pointer of the saved context of the
   *   test task before the arrangement.
   */
  uintptr_t saved_stack_pointer;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

    /**
     * @brief This member defines the representative index for the next action.
     */
    size_t rep;

    /**
     * @brief If this member is true, then the current run of the
     *   representatives should be skipped.
     */
    bool skip_rep;

    /**
     * @brief This member records the representatives which ran.
     */
    bool rep_ran[ 22 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsStackchkReqVisit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsStackchkReqVisit_Context;

static RtemsStackchkReqVisit_Context RtemsStackchkReqVisit_Instance;

static const char *const RtemsStackchkReqVisit_PreDesc_Enabled[] =
  { "Yes", "No", "NA" };

static const char *const RtemsStackchkReqVisit_PreDesc_Kind[] = {
  "ThreadShort",
  "ThreadBelowLimit",
  "ThreadLimit",
  "ThreadAboveLimit",
  "ThreadLong",
  "ISR",
  "NA"
};

static const char *const RtemsStackchkReqVisit_PreDesc_HasExecuted[] =
  { "YesMiddle", "YesDeepest", "No", "NA" };

static const char *const RtemsStackchkReqVisit_PreDesc_StackPointer[] = {
  "BelowMin",
  "BelowEdge",
  "InsideFirst",
  "InsideMiddle",
  "InsideLast",
  "AboveEdge",
  "AboveMax",
  "NA"
};

static const char *const *const RtemsStackchkReqVisit_PreDesc[] = {
  RtemsStackchkReqVisit_PreDesc_Enabled,
  RtemsStackchkReqVisit_PreDesc_Kind,
  RtemsStackchkReqVisit_PreDesc_HasExecuted,
  RtemsStackchkReqVisit_PreDesc_StackPointer,
  NULL
};

typedef enum {
  RtemsStackchkReqVisit_Pre_Kind_Thread_Short,
  RtemsStackchkReqVisit_Pre_Kind_Thread_BelowLimit,
  RtemsStackchkReqVisit_Pre_Kind_Thread_Limit,
  RtemsStackchkReqVisit_Pre_Kind_Thread_AboveLimit,
  RtemsStackchkReqVisit_Pre_Kind_Thread_Long
} RtemsStackchkReqVisit_Pre_Kind_Thread_Rep;

typedef enum {
  RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Middle,
  RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Deepest
} RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Rep;

typedef enum {
  RtemsStackchkReqVisit_Pre_StackPointer_Below_Min,
  RtemsStackchkReqVisit_Pre_StackPointer_Below_Edge
} RtemsStackchkReqVisit_Pre_StackPointer_Below_Rep;

typedef enum {
  RtemsStackchkReqVisit_Pre_StackPointer_Inside_First,
  RtemsStackchkReqVisit_Pre_StackPointer_Inside_Middle,
  RtemsStackchkReqVisit_Pre_StackPointer_Inside_Last
} RtemsStackchkReqVisit_Pre_StackPointer_Inside_Rep;

typedef enum {
  RtemsStackchkReqVisit_Pre_StackPointer_Above_Edge,
  RtemsStackchkReqVisit_Pre_StackPointer_Above_Max
} RtemsStackchkReqVisit_Pre_StackPointer_Above_Rep;

static const uint8_t RtemsStackchkReqVisit_PreRepOffset_Enabled[] =
  { 0, 1, 2, 3 };

static const uint8_t RtemsStackchkReqVisit_PreRepOffset_Kind[] =
  { 0, 5, 6, 7 };

static const uint8_t RtemsStackchkReqVisit_PreRepOffset_HasExecuted[] =
  { 0, 2, 3, 4 };

static const uint8_t RtemsStackchkReqVisit_PreRepOffset_StackPointer[] =
  { 0, 2, 5, 7, 8 };

static const uint8_t *const RtemsStackchkReqVisit_PreRepOffset[] = {
  RtemsStackchkReqVisit_PreRepOffset_Enabled,
  RtemsStackchkReqVisit_PreRepOffset_Kind,
  RtemsStackchkReqVisit_PreRepOffset_HasExecuted,
  RtemsStackchkReqVisit_PreRepOffset_StackPointer
};

static size_t RtemsStackchkReqVisit_GetRepresentativeCountOf(
  const RtemsStackchkReqVisit_Context *ctx,
  size_t                               index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsStackchkReqVisit_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsStackchkReqVisit_GetRepresentative(
  const RtemsStackchkReqVisit_Context *ctx,
  size_t                               index
)
{
  size_t count;

  count = RtemsStackchkReqVisit_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsStackchkReqVisit_GetRepresentativeCount(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 4; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsStackchkReqVisit_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsStackchkReqVisit_Pre_Kind_Thread_Rep
RtemsStackchkReqVisit_Pre_Kind_Thread_GetRepresentative(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  return (
    RtemsStackchkReqVisit_Pre_Kind_Thread_Rep
  ) RtemsStackchkReqVisit_GetRepresentative( ctx, 1 );
}

static RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Rep
RtemsStackchkReqVisit_Pre_HasExecuted_Yes_GetRepresentative(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  return (
    RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Rep
  ) RtemsStackchkReqVisit_GetRepresentative( ctx, 2 );
}

static RtemsStackchkReqVisit_Pre_StackPointer_Below_Rep
RtemsStackchkReqVisit_Pre_StackPointer_Below_GetRepresentative(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  return (
    RtemsStackchkReqVisit_Pre_StackPointer_Below_Rep
  ) RtemsStackchkReqVisit_GetRepresentative( ctx, 3 );
}

static RtemsStackchkReqVisit_Pre_StackPointer_Inside_Rep
RtemsStackchkReqVisit_Pre_StackPointer_Inside_GetRepresentative(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  return (
    RtemsStackchkReqVisit_Pre_StackPointer_Inside_Rep
  ) RtemsStackchkReqVisit_GetRepresentative( ctx, 3 );
}

static RtemsStackchkReqVisit_Pre_StackPointer_Above_Rep
RtemsStackchkReqVisit_Pre_StackPointer_Above_GetRepresentative(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  return (
    RtemsStackchkReqVisit_Pre_StackPointer_Above_Rep
  ) RtemsStackchkReqVisit_GetRepresentative( ctx, 3 );
}

static const uint8_t RtemsStackchkReqVisit_PreRepBase[] = { 0, 3, 10, 14 };

static const uint8_t RtemsStackchkReqVisit_PreRepStates[] = { 2, 2, 2, 3 };

static void RtemsStackchkReqVisit_ResetRepresentatives(
  RtemsStackchkReqVisit_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 22; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsStackchkReqVisit_MarkRepresentatives(
  RtemsStackchkReqVisit_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 4; ++i ) {
    size_t rep;

    rep = RtemsStackchkReqVisit_PreRepBase[ i ] +
          RtemsStackchkReqVisit_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsStackchkReqVisit_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsStackchkReqVisit_CheckRepresentatives(
  const RtemsStackchkReqVisit_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 4; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsStackchkReqVisit_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsStackchkReqVisit_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsStackchkReqVisit_PreRepStates[ i ]; ++state
    ) {
      size_t k;
      bool   any;

      any = false;

      for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
        any = any || ran[ k ];
      }

      if ( any ) {
        for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
          T_quiet_true(
            ran[ k ],
            "representative %s did not run",
            RtemsStackchkReqVisit_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef RtemsStackchkReqVisit_Context Context;

static bool IsStackCheckerEnabled( void )
{
  size_t i;

  for ( i = 0; i < _User_extensions_Initial_count; ++i ) {
    if (
      _User_extensions_Initial_extensions[ i ].thread_create ==
      rtems_stack_checker_create_extension
    ) {
      return true;
    }
  }

  return false;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->executed = true;

  while ( true ) {
    (void) ReceiveAnyEvents();
  }
}

static void SetSavedStackPointer( Context *ctx, uintptr_t stack_pointer )
{
  Context_Control *context;
  Context_Control  saved;
  size_t           i;

  context = &GetThread( ctx->task_id )->Registers;
  saved = *context;

  /*
   * The member of the stack pointer in the context depends on the
   * architecture and on some architectures also on the address of the
   * context.  Each byte of the context which _CPU_Context_Get_SP() reads
   * receives the corresponding byte of the stack pointer.
   */
  for ( i = 0; i < sizeof( *context ); ++i ) {
    uintptr_t value;
    size_t    shift;

    memset( context, 0, sizeof( *context ) );
    ( (uint8_t *) context )[ i ] = 0xff;
    value = _CPU_Context_Get_SP( context );

    for ( shift = 0; shift < 8 * sizeof( value ); shift += 8 ) {
      if ( ( ( value >> shift ) & 0xff ) == 0xff ) {
        ( (uint8_t *) &saved )[ i ] = (uint8_t) ( stack_pointer >> shift );
        break;
      }
    }
  }

  *context = saved;
}

/* The stack checker fills each stack with this value */
#define STACK_FILL_PATTERN 0xa5a5a5a5U

static void SetName( Context *ctx )
{
  Thread_Control *thread;
  char            name[ 4 * NAME_SIZE ];

  T_lt_sz( ctx->name_length, sizeof( name ) );
  memset( name, 'N', ctx->name_length );
  name[ ctx->name_length ] = '\0';

  thread = GetThread( ctx->task_id );
  (void) _Thread_Set_name( thread, name );
  (void) _Thread_Get_name( thread, name, sizeof( name ) );
  strlcpy( ctx->expected_name, name, NAME_SIZE );

  if ( _Thread_Maximum_name_size > ctx->name_length ) {
    T_eq_sz( strlen( name ), ctx->name_length );
  }
}

/*
 * Write zero to the word at the middle or to the deepest word of the usable
 * stack area.
 */
static void ArrangeUse( Context *ctx )
{
  uintptr_t             begin;
  uintptr_t             size;
  uint32_t             *word;
  rtems_interrupt_level level;

#if CPU_STACK_GROWS_UP == TRUE
  begin = ctx->area_begin;
  size = ctx->area_end - begin - STACK_CHECK_PATTERN_SIZE;

  if ( ctx->use_deepest ) {
    ctx->used_depth = size - sizeof( *word );
  } else {
    ctx->used_depth = RTEMS_ALIGN_DOWN( size / 2, sizeof( *word ) );
  }

  word = (uint32_t *) ( begin + ctx->used_depth );
#else
  begin = ctx->area_end;
  size = begin - ctx->area_begin - STACK_CHECK_PATTERN_SIZE;

  if ( ctx->use_deepest ) {
    ctx->used_depth = size;
  } else {
    ctx->used_depth = RTEMS_ALIGN_DOWN( size / 2, sizeof( *word ) );
  }

  word = (uint32_t *) ( begin - ctx->used_depth );
#endif

  rtems_interrupt_local_disable( level );
  ctx->used_word = word;
  ctx->used_word_value = *word;
  *word = 0;
  rtems_interrupt_local_enable( level );
}

/*
 * Restore the word which ArrangeUse() marked.  A marked interrupt stack
 * would otherwise show this use to the later visits.
 */
static void RestoreUse( Context *ctx )
{
  rtems_interrupt_level level;

  if ( ctx->used_word != NULL ) {
    rtems_interrupt_local_disable( level );
    *ctx->used_word = ctx->used_word_value;
    rtems_interrupt_local_enable( level );
    ctx->used_word = NULL;
  }
}

static void ArrangeInterruptStackUse( void )
{
  Per_CPU_Control      *cpu;
  uint32_t             *word;
  rtems_interrupt_level level;

  cpu = _Per_CPU_Get_by_index( 0 );
#if CPU_STACK_GROWS_UP == TRUE
  word = (uint32_t *) cpu->interrupt_stack_low + 1;
#else
  word = (uint32_t *) cpu->interrupt_stack_high - 1;
#endif

  rtems_interrupt_local_disable( level );

  if ( *word == STACK_FILL_PATTERN ) {
    *word = 0;
  }

  rtems_interrupt_local_enable( level );
}

/*
 * The test task does not execute until the action restores the stack pointer
 * of its saved context.
 */
static void ArrangeStackPointer( Context *ctx )
{
  uintptr_t stack_pointer;

  ctx->saved_stack_pointer = (uintptr_t) _CPU_Context_Get_SP(
    &GetThread( ctx->task_id )->Registers
  );

  switch ( ctx->position ) {
    case POSITION_BELOW:
      if ( ctx->point == POINT_FAR ) {
        stack_pointer = 0;
      } else {
        stack_pointer = ctx->area_begin - CPU_STACK_ALIGNMENT;
      }
      break;
    case POSITION_ABOVE:
      if ( ctx->point == POINT_FAR ) {
        stack_pointer = RTEMS_ALIGN_DOWN( UINTPTR_MAX, CPU_STACK_ALIGNMENT );
      } else {
        stack_pointer = ctx->area_end + CPU_STACK_ALIGNMENT;
      }
      break;
    default:
      if ( ctx->point == POINT_FIRST ) {
        stack_pointer = ctx->area_begin;
      } else if ( ctx->point == POINT_LAST ) {
        stack_pointer = ctx->area_end - CPU_STACK_ALIGNMENT;
      } else {
        stack_pointer = RTEMS_ALIGN_DOWN(
          ctx->area_begin + ( ctx->area_end - ctx->area_begin ) / 2,
          CPU_STACK_ALIGNMENT
        );
      }
      break;
  }

  SetSavedStackPointer( ctx, stack_pointer );
  ctx->stack_pointer = stack_pointer;
}

static void Visit( const rtems_stack_checker_info *info, void *arg )
{
  Context *ctx;
  bool     selected;

  ctx = arg;

  if ( ctx->kind == KIND_THREAD ) {
    selected = info->id == ctx->task_id;
  } else {
    /* The identifier of an interrupt stack is a processor index */
    selected = rtems_object_id_get_api( info->id ) == 0 && info->id == 0;
  }

  if ( selected ) {
    ++ctx->visits;
    ctx->info = *info;
    strlcpy( ctx->name, info->name, sizeof( ctx->name ) );
  }
}

static void RtemsStackchkReqVisit_Pre_Enabled_Prepare(
  RtemsStackchkReqVisit_Context    *ctx,
  RtemsStackchkReqVisit_Pre_Enabled state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Pre_Enabled_Yes: {
      /*
       * Where the application configuration enables the stack checker.
       */
      if ( !IsStackCheckerEnabled() ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsStackchkReqVisit_Pre_Enabled_No: {
      /*
       * Where the application configuration does not enable the stack checker.
       */
      if ( IsStackCheckerEnabled() ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsStackchkReqVisit_Pre_Enabled_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Pre_Kind_Prepare(
  RtemsStackchkReqVisit_Context *ctx,
  RtemsStackchkReqVisit_Pre_Kind state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Pre_Kind_Thread: {
      /*
       * While the stack is the stack of a thread.
       */
      switch (
        RtemsStackchkReqVisit_Pre_Kind_Thread_GetRepresentative( ctx )
      ) {
        case RtemsStackchkReqVisit_Pre_Kind_Thread_Short: {
          ctx->name_length = 1;
          break;
        }

        case RtemsStackchkReqVisit_Pre_Kind_Thread_BelowLimit: {
          ctx->name_length = NAME_SIZE - 2;
          break;
        }

        case RtemsStackchkReqVisit_Pre_Kind_Thread_Limit: {
          ctx->name_length = NAME_SIZE - 1;
          break;
        }

        case RtemsStackchkReqVisit_Pre_Kind_Thread_AboveLimit: {
          ctx->name_length = NAME_SIZE;
          break;
        }

        case RtemsStackchkReqVisit_Pre_Kind_Thread_Long: {
          ctx->name_length = 31;
          break;
        }
      }

      ctx->kind = KIND_THREAD;
      break;
    }

    case RtemsStackchkReqVisit_Pre_Kind_ISR: {
      /*
       * While the stack is the interrupt stack of a processor.
       */
      ctx->kind = KIND_ISR;
      break;
    }

    case RtemsStackchkReqVisit_Pre_Kind_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Pre_HasExecuted_Prepare(
  RtemsStackchkReqVisit_Context        *ctx,
  RtemsStackchkReqVisit_Pre_HasExecuted state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Pre_HasExecuted_Yes: {
      /*
       * While the owner of the stack executed on the stack.
       */
      switch (
        RtemsStackchkReqVisit_Pre_HasExecuted_Yes_GetRepresentative( ctx )
      ) {
        case RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Middle: {
          ctx->use_deepest = false;
          break;
        }

        case RtemsStackchkReqVisit_Pre_HasExecuted_Yes_Deepest: {
          ctx->use_deepest = true;
          break;
        }
      }

      ctx->execute = true;
      break;
    }

    case RtemsStackchkReqVisit_Pre_HasExecuted_No: {
      /*
       * While the owner of the stack never executed on the stack.
       */
      ctx->execute = false;
      break;
    }

    case RtemsStackchkReqVisit_Pre_HasExecuted_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Pre_StackPointer_Prepare(
  RtemsStackchkReqVisit_Context         *ctx,
  RtemsStackchkReqVisit_Pre_StackPointer state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Pre_StackPointer_Below: {
      /*
       * While the stack pointer of the saved context of the thread is below
       * the begin address of the allocated stack area of the thread.
       */
      switch (
        RtemsStackchkReqVisit_Pre_StackPointer_Below_GetRepresentative( ctx )
      ) {
        case RtemsStackchkReqVisit_Pre_StackPointer_Below_Min: {
          ctx->point = POINT_FAR;
          break;
        }

        case RtemsStackchkReqVisit_Pre_StackPointer_Below_Edge: {
          ctx->point = POINT_EDGE;
          break;
        }
      }

      ctx->position = POSITION_BELOW;
      break;
    }

    case RtemsStackchkReqVisit_Pre_StackPointer_Inside: {
      /*
       * While the stack pointer of the saved context of the thread is inside
       * the allocated stack area of the thread.
       */
      switch (
        RtemsStackchkReqVisit_Pre_StackPointer_Inside_GetRepresentative( ctx )
      ) {
        case RtemsStackchkReqVisit_Pre_StackPointer_Inside_First: {
          ctx->point = POINT_FIRST;
          break;
        }

        case RtemsStackchkReqVisit_Pre_StackPointer_Inside_Middle: {
          ctx->point = POINT_MIDDLE;
          break;
        }

        case RtemsStackchkReqVisit_Pre_StackPointer_Inside_Last: {
          ctx->point = POINT_LAST;
          break;
        }
      }

      ctx->position = POSITION_INSIDE;
      break;
    }

    case RtemsStackchkReqVisit_Pre_StackPointer_Above: {
      /*
       * While the stack pointer of the saved context of the thread is above
       * the end address of the allocated stack area of the thread.
       */
      switch (
        RtemsStackchkReqVisit_Pre_StackPointer_Above_GetRepresentative( ctx )
      ) {
        case RtemsStackchkReqVisit_Pre_StackPointer_Above_Edge: {
          ctx->point = POINT_EDGE;
          break;
        }

        case RtemsStackchkReqVisit_Pre_StackPointer_Above_Max: {
          ctx->point = POINT_FAR;
          break;
        }
      }

      ctx->position = POSITION_ABOVE;
      break;
    }

    case RtemsStackchkReqVisit_Pre_StackPointer_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Post_Id_Check(
  RtemsStackchkReqVisit_Context *ctx,
  RtemsStackchkReqVisit_Post_Id  state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Post_Id_Thread: {
      /*
       * The id member of the stack information shall be the object identifier
       * of the thread.
       */
      T_eq_u32( ctx->info.id, ctx->task_id );
      break;
    }

    case RtemsStackchkReqVisit_Post_Id_Processor: {
      /*
       * The id member of the stack information shall be the index of the
       * processor of the interrupt stack.
       */
      T_eq_u32( ctx->info.id, 0 );
      break;
    }

    case RtemsStackchkReqVisit_Post_Id_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Post_Name_Check(
  RtemsStackchkReqVisit_Context  *ctx,
  RtemsStackchkReqVisit_Post_Name state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Post_Name_Thread: {
      /*
       * The name member of the stack information shall reference the name of
       * the thread truncated to at most 21 characters.
       */
      T_eq_str( ctx->name, ctx->expected_name );
      break;
    }

    case RtemsStackchkReqVisit_Post_Name_Interrupt: {
      /*
       * The name member of the stack information shall reference the string
       * `Interrupt Stack`.
       */
      T_eq_str( ctx->name, "Interrupt Stack" );
      break;
    }

    case RtemsStackchkReqVisit_Post_Name_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Post_Area_Check(
  RtemsStackchkReqVisit_Context  *ctx,
  RtemsStackchkReqVisit_Post_Area state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Post_Area_Allocated: {
      /*
       * The begin and size members of the stack information shall reference
       * the allocated stack area of the stack without the stack checker
       * pattern area.
       */
      uintptr_t begin;
      uintptr_t end;

      begin = (uintptr_t) ctx->info.begin;
      end = begin + ctx->info.size;
      #if CPU_STACK_GROWS_UP == TRUE
      T_eq_uptr( begin, ctx->area_begin );
      T_lt_uptr( end, ctx->area_end );
      #else
      T_gt_uptr( begin, ctx->area_begin );
      T_eq_uptr( end, ctx->area_end );
      #endif
      break;
    }

    case RtemsStackchkReqVisit_Post_Area_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Post_StackPointer_Check(
  RtemsStackchkReqVisit_Context          *ctx,
  RtemsStackchkReqVisit_Post_StackPointer state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Post_StackPointer_Context: {
      /*
       * The current member of the stack information shall be the stack pointer
       * of the saved context of the thread.
       */
      T_eq_uptr( (uintptr_t) ctx->info.current, ctx->stack_pointer );
      break;
    }

    case RtemsStackchkReqVisit_Post_StackPointer_Null: {
      /*
       * The current member of the stack information shall be NULL.
       */
      T_null( ctx->info.current );
      break;
    }

    case RtemsStackchkReqVisit_Post_StackPointer_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Post_Used_Check(
  RtemsStackchkReqVisit_Context  *ctx,
  RtemsStackchkReqVisit_Post_Used state
)
{
  switch ( state ) {
    case RtemsStackchkReqVisit_Post_Used_Zero: {
      /*
       * The used member of the stack information shall be zero.
       */
      T_eq_uptr( ctx->info.used, 0 );
      break;
    }

    case RtemsStackchkReqVisit_Post_Used_Deepest: {
      /*
       * The used member of the stack information shall be the distance in
       * bytes from the begin of the used stack area to the word of the stack
       * which does not hold the fill pattern and which is farthest from this
       * begin in the direction of stack growth.
       */
      if ( ctx->kind == KIND_THREAD || ctx->use_deepest ) {
        T_eq_uptr( ctx->info.used, ctx->used_depth );
      } else {
        T_ge_uptr( ctx->info.used, ctx->used_depth );
        T_lt_uptr( ctx->info.used, ctx->info.size );
      }
      break;
    }

    case RtemsStackchkReqVisit_Post_Used_Unknown: {
      /*
       * The used member of the stack information shall be UINTPTR_MAX.
       */
      T_eq_uptr( ctx->info.used, UINTPTR_MAX );
      break;
    }

    case RtemsStackchkReqVisit_Post_Used_NA:
      break;
  }
}

static void RtemsStackchkReqVisit_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void RtemsStackchkReqVisit_Setup_Wrap( void *arg )
{
  RtemsStackchkReqVisit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsStackchkReqVisit_Setup();
}

static void RtemsStackchkReqVisit_Teardown( void )
{
  RestoreRunnerPriority();
}

static void RtemsStackchkReqVisit_Teardown_Wrap( void *arg )
{
  RtemsStackchkReqVisit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsStackchkReqVisit_Teardown();
}

static void RtemsStackchkReqVisit_Prepare( RtemsStackchkReqVisit_Context *ctx )
{
  ctx->task_id = 0;
  ctx->used_word = NULL;
}

static void RtemsStackchkReqVisit_Action( RtemsStackchkReqVisit_Context *ctx )
{
  ctx->executed = false;
  ctx->task_id = CreateTask( "STCK", PRIO_HIGH );

  if ( ctx->kind == KIND_THREAD ) {
    const Stack_Control *stack;

    stack = &GetThread( ctx->task_id )->Start.Initial_stack;
    ctx->area_begin = (uintptr_t) stack->area;
    ctx->area_end = ctx->area_begin + stack->size;

    SetName( ctx );

    if ( ctx->execute ) {
      StartTask( ctx->task_id, Worker, ctx );

      while ( !ctx->executed ) {
        /* Wait */
      }

      ArrangeUse( ctx );
    }

    ArrangeStackPointer( ctx );
  } else {
    const Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( 0 );
    ctx->area_begin = (uintptr_t) cpu->interrupt_stack_low;
    ctx->area_end = (uintptr_t) cpu->interrupt_stack_high;

    if ( ctx->execute ) {
      ArrangeInterruptStackUse();
      ArrangeUse( ctx );
    }
  }

  ctx->visits = 0;
  rtems_stack_checker_iterate( Visit, ctx );
  T_eq_u32( ctx->visits, 1 );
  RestoreUse( ctx );

  if ( ctx->kind == KIND_THREAD ) {
    SetSavedStackPointer( ctx, ctx->saved_stack_pointer );
  }
}

static void RtemsStackchkReqVisit_Cleanup( RtemsStackchkReqVisit_Context *ctx )
{
  DeleteTask( ctx->task_id );
}

/* clang-format off */

static const RtemsStackchkReqVisit_Entry
RtemsStackchkReqVisit_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsStackchkReqVisit_Post_Id_Thread,
    RtemsStackchkReqVisit_Post_Name_Thread,
    RtemsStackchkReqVisit_Post_Area_Allocated,
    RtemsStackchkReqVisit_Post_StackPointer_Context,
    RtemsStackchkReqVisit_Post_Used_Unknown },
  { 1, 0, 0, 0, 1, RtemsStackchkReqVisit_Post_Id_NA,
    RtemsStackchkReqVisit_Post_Name_NA, RtemsStackchkReqVisit_Post_Area_NA,
    RtemsStackchkReqVisit_Post_StackPointer_NA,
    RtemsStackchkReqVisit_Post_Used_NA },
  { 0, 0, 0, 0, 0, RtemsStackchkReqVisit_Post_Id_Thread,
    RtemsStackchkReqVisit_Post_Name_Thread,
    RtemsStackchkReqVisit_Post_Area_Allocated,
    RtemsStackchkReqVisit_Post_StackPointer_Context,
    RtemsStackchkReqVisit_Post_Used_Deepest },
  { 0, 0, 0, 0, 0, RtemsStackchkReqVisit_Post_Id_Thread,
    RtemsStackchkReqVisit_Post_Name_Thread,
    RtemsStackchkReqVisit_Post_Area_Allocated,
    RtemsStackchkReqVisit_Post_StackPointer_Context,
    RtemsStackchkReqVisit_Post_Used_Zero },
  { 1, 0, 0, 0, 1, RtemsStackchkReqVisit_Post_Id_NA,
    RtemsStackchkReqVisit_Post_Name_NA, RtemsStackchkReqVisit_Post_Area_NA,
    RtemsStackchkReqVisit_Post_StackPointer_NA,
    RtemsStackchkReqVisit_Post_Used_NA },
  { 1, 0, 0, 0, 0, RtemsStackchkReqVisit_Post_Id_NA,
    RtemsStackchkReqVisit_Post_Name_NA, RtemsStackchkReqVisit_Post_Area_NA,
    RtemsStackchkReqVisit_Post_StackPointer_NA,
    RtemsStackchkReqVisit_Post_Used_NA },
  { 0, 0, 0, 0, 1, RtemsStackchkReqVisit_Post_Id_Processor,
    RtemsStackchkReqVisit_Post_Name_Interrupt,
    RtemsStackchkReqVisit_Post_Area_Allocated,
    RtemsStackchkReqVisit_Post_StackPointer_Null,
    RtemsStackchkReqVisit_Post_Used_Deepest }
};

static const uint8_t
RtemsStackchkReqVisit_Map[] = {
  2, 2, 2, 3, 3, 3, 6, 5, 5, 4, 4, 4, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t RtemsStackchkReqVisit_Scope( void *arg, char *buf, size_t n )
{
  RtemsStackchkReqVisit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 4 ];
    size_t i;

    for ( i = 0; i < 4; ++i ) {
      pcd[ i ] = RtemsStackchkReqVisit_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 RtemsStackchkReqVisit_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsStackchkReqVisit_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsStackchkReqVisit_Fixture = {
  .setup = RtemsStackchkReqVisit_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsStackchkReqVisit_Teardown_Wrap,
  .scope = RtemsStackchkReqVisit_Scope,
  .initial_context = &RtemsStackchkReqVisit_Instance
};

static const uint8_t RtemsStackchkReqVisit_Weights[] = { 12, 6, 3, 1 };

static void RtemsStackchkReqVisit_Skip(
  RtemsStackchkReqVisit_Context *ctx,
  size_t                         index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = RtemsStackchkReqVisit_Pre_Kind_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = RtemsStackchkReqVisit_Pre_HasExecuted_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = RtemsStackchkReqVisit_Pre_StackPointer_NA - 1;
      break;
  }
}

static inline RtemsStackchkReqVisit_Entry RtemsStackchkReqVisit_PopEntry(
  RtemsStackchkReqVisit_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 4; ++i ) {
      index += RtemsStackchkReqVisit_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsStackchkReqVisit_Entries[ RtemsStackchkReqVisit_Map[ index ] ];
}

static void RtemsStackchkReqVisit_SetPreConditionStates(
  RtemsStackchkReqVisit_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_StackPointer_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsStackchkReqVisit_Pre_StackPointer_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void RtemsStackchkReqVisit_TestVariant(
  RtemsStackchkReqVisit_Context *ctx
)
{
  RtemsStackchkReqVisit_Pre_Enabled_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    RtemsStackchkReqVisit_Skip( ctx, 0 );
    return;
  }

  RtemsStackchkReqVisit_Pre_Kind_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsStackchkReqVisit_Pre_HasExecuted_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsStackchkReqVisit_Pre_StackPointer_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsStackchkReqVisit_Action( ctx );
  RtemsStackchkReqVisit_Post_Id_Check( ctx, ctx->Map.entry.Post_Id );
  RtemsStackchkReqVisit_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsStackchkReqVisit_Post_Area_Check( ctx, ctx->Map.entry.Post_Area );
  RtemsStackchkReqVisit_Post_StackPointer_Check(
    ctx,
    ctx->Map.entry.Post_StackPointer
  );
  RtemsStackchkReqVisit_Post_Used_Check( ctx, ctx->Map.entry.Post_Used );
}

/**
 * @fn void T_case_body_RtemsStackchkReqVisit( void )
 */
T_TEST_CASE_FIXTURE( RtemsStackchkReqVisit, &RtemsStackchkReqVisit_Fixture )
{
  RtemsStackchkReqVisit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  RtemsStackchkReqVisit_ResetRepresentatives( ctx );

  for (
    ctx->Map.pci[ 0 ] = RtemsStackchkReqVisit_Pre_Enabled_Yes;
    ctx->Map.pci[ 0 ] < RtemsStackchkReqVisit_Pre_Enabled_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsStackchkReqVisit_Pre_Kind_Thread;
      ctx->Map.pci[ 1 ] < RtemsStackchkReqVisit_Pre_Kind_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsStackchkReqVisit_Pre_HasExecuted_Yes;
        ctx->Map.pci[ 2 ] < RtemsStackchkReqVisit_Pre_HasExecuted_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsStackchkReqVisit_Pre_StackPointer_Below;
          ctx->Map.pci[ 3 ] < RtemsStackchkReqVisit_Pre_StackPointer_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsStackchkReqVisit_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsStackchkReqVisit_SetPreConditionStates( ctx );

          for (
            ctx->Map.rep = 0;
            ctx->Map.rep < RtemsStackchkReqVisit_GetRepresentativeCount( ctx );
            ++ctx->Map.rep
          ) {
            ctx->Map.skip_rep = false;
            RtemsStackchkReqVisit_Prepare( ctx );
            RtemsStackchkReqVisit_TestVariant( ctx );
            RtemsStackchkReqVisit_Cleanup( ctx );

            if ( ctx->Map.skip ) {
              break;
            }

            RtemsStackchkReqVisit_MarkRepresentatives( ctx );
          }
        }
      }
    }
  }

  RtemsStackchkReqVisit_CheckRepresentatives( ctx );
}

/** @} */
