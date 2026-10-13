/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsStackchkReqIsBlown
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
#include <setjmp.h>
#include <string.h>
#include <rtems/stackchk.h>
#include <rtems/stackchkimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsStackchkReqIsBlown spec:/rtems/stackchk/req/is-blown
 *
 * @ingroup TestsuitesValidation0
 * @ingroup TestsuitesValidationSysinit0
 *
 * State Below of a stack which grows down uses a buffer in the stack frame of
 * the action. State Above of a stack which grows up uses the same buffer. The
 * action passes the buffer to a function which it calls. A local variable of
 * that function is therefore beyond the buffer in the direction of stack
 * growth. The case takes the address of this variable as the stack pointer of
 * the caller.
 *
 * The test suite of each context runs the test case. The test case skips the
 * states of dimension Context which do not match the system state. In the
 * system initialization context, the checked stack is the interrupt stack of
 * the current processor. The action sets the interrupt stack bounds of this
 * processor to the arranged allocated stack area. The interrupt processing
 * uses these bounds, so the action disables interrupts until it restores them.
 *
 * @{
 */

typedef enum {
  RtemsStackchkReqIsBlown_Pre_Context_Multitasking,
  RtemsStackchkReqIsBlown_Pre_Context_Initialization,
  RtemsStackchkReqIsBlown_Pre_Context_NA
} RtemsStackchkReqIsBlown_Pre_Context;

typedef enum {
  RtemsStackchkReqIsBlown_Pre_StackPointer_Below,
  RtemsStackchkReqIsBlown_Pre_StackPointer_Inside,
  RtemsStackchkReqIsBlown_Pre_StackPointer_Above,
  RtemsStackchkReqIsBlown_Pre_StackPointer_NA
} RtemsStackchkReqIsBlown_Pre_StackPointer;

typedef enum {
  RtemsStackchkReqIsBlown_Pre_Pattern_Intact,
  RtemsStackchkReqIsBlown_Pre_Pattern_Damaged,
  RtemsStackchkReqIsBlown_Pre_Pattern_NA
} RtemsStackchkReqIsBlown_Pre_Pattern;

typedef enum {
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Intact,
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged,
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_NA
} RtemsStackchkReqIsBlown_Pre_InterruptStackPattern;

typedef enum {
  RtemsStackchkReqIsBlown_Post_Result_False,
  RtemsStackchkReqIsBlown_Post_Result_NoReturn,
  RtemsStackchkReqIsBlown_Post_Result_NA
} RtemsStackchkReqIsBlown_Post_Result;

typedef enum {
  RtemsStackchkReqIsBlown_Post_Fatal_Nop,
  RtemsStackchkReqIsBlown_Post_Fatal_StackChecker,
  RtemsStackchkReqIsBlown_Post_Fatal_StackCheckerInterrupt,
  RtemsStackchkReqIsBlown_Post_Fatal_NA
} RtemsStackchkReqIsBlown_Post_Fatal;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Context_NA : 1;
  uint16_t Pre_StackPointer_NA : 1;
  uint16_t Pre_Pattern_NA : 1;
  uint16_t Pre_InterruptStackPattern_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_Fatal : 2;
} RtemsStackchkReqIsBlown_Entry;

typedef enum { POSITION_BELOW, POSITION_INSIDE, POSITION_ABOVE } Position;

/**
 * @brief Test context for spec:/rtems/stackchk/req/is-blown test case.
 */
typedef struct {
  /**
   * @brief This member is true, if the test runs in the system initialization
   *   context.
   */
  bool initialization;

  /**
   * @brief This member references the checked stack.
   */
  Stack_Control *stack;

  /**
   * @brief This member contains a copy of the checked stack before the
   *   arrangement.
   */
  Stack_Control saved_stack;

  /**
   * @brief This member contains the interrupt stack of the current processor.
   */
  Stack_Control interrupt_stack;

  /**
   * @brief This member contains the saved interrupt stack begin of the current
   *   processor.
   */
  void *interrupt_stack_low;

  /**
   * @brief This member contains the saved interrupt stack end of the current
   *   processor.
   */
  void *interrupt_stack_high;

  /**
   * @brief This member specifies the position of the stack pointer of the
   *   caller relative to the allocated stack area of the checked stack.
   */
  Position position;

  /**
   * @brief This member is true, if the pattern area of the stack of the
   *   executing thread shall be damaged.
   */
  bool damage;

  /**
   * @brief This member contains the offset of the damaged byte in the pattern
   *   area of the stack of the executing thread.
   */
  size_t damage_offset;

  /**
   * @brief This member contains the offset of the damaged byte in the pattern
   *   area of the interrupt stack of the current processor.
   */
  size_t interrupt_damage_offset;

  /**
   * @brief This member is true, if the pattern area of the interrupt stack of
   *   the current processor shall be damaged.
   */
  bool damage_interrupt_stack;

  /**
   * @brief This member contains the processor of the thread dispatch disable.
   */
  Per_CPU_Control *cpu_self;

  /**
   * @brief This member provides the context to return from the fatal error.
   */
  jmp_buf before;

  /**
   * @brief This member is true, if the rtems_stack_checker_is_blown() call
   *   returned.
   */
  bool returned;

  /**
   * @brief This member contains the return value of the
   *   rtems_stack_checker_is_blown() call.
   */
  bool result;

  /**
   * @brief This member contains the count of fatal errors.
   */
  uint32_t fatal_counter;

  /**
   * @brief This member contains the fatal source of the last fatal error.
   */
  rtems_fatal_source fatal_source;

  /**
   * @brief This member contains the fatal code of the last fatal error.
   */
  rtems_fatal_code fatal_code;

  /**
   * @brief This member contains the object name of the executing thread.
   */
  rtems_name name;

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
    bool rep_ran[ 15 ];

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
    RtemsStackchkReqIsBlown_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsStackchkReqIsBlown_Context;

static RtemsStackchkReqIsBlown_Context RtemsStackchkReqIsBlown_Instance;

static const char *const RtemsStackchkReqIsBlown_PreDesc_Context[] =
  { "Multitasking", "Initialization", "NA" };

static const char *const RtemsStackchkReqIsBlown_PreDesc_StackPointer[] =
  { "Below", "Inside", "Above", "NA" };

static const char *const RtemsStackchkReqIsBlown_PreDesc_Pattern[] =
  { "Intact", "DamagedFirst", "DamagedLast", "NA" };

static const char *const
  RtemsStackchkReqIsBlown_PreDesc_InterruptStackPattern[] =
    { "Intact", "DamagedFirst", "DamagedLast", "NA" };

static const char *const *const RtemsStackchkReqIsBlown_PreDesc[] = {
  RtemsStackchkReqIsBlown_PreDesc_Context,
  RtemsStackchkReqIsBlown_PreDesc_StackPointer,
  RtemsStackchkReqIsBlown_PreDesc_Pattern,
  RtemsStackchkReqIsBlown_PreDesc_InterruptStackPattern,
  NULL
};

typedef enum {
  RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_First,
  RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_Last
} RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_Rep;

typedef enum {
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_First,
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_Last
} RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_Rep;

static const uint8_t RtemsStackchkReqIsBlown_PreRepOffset_Context[] =
  { 0, 1, 2, 3 };

static const uint8_t RtemsStackchkReqIsBlown_PreRepOffset_StackPointer[] =
  { 0, 1, 2, 3, 4 };

static const uint8_t RtemsStackchkReqIsBlown_PreRepOffset_Pattern[] =
  { 0, 1, 3, 4 };

static const uint8_t
  RtemsStackchkReqIsBlown_PreRepOffset_InterruptStackPattern[] =
    { 0, 1, 3, 4 };

static const uint8_t *const RtemsStackchkReqIsBlown_PreRepOffset[] = {
  RtemsStackchkReqIsBlown_PreRepOffset_Context,
  RtemsStackchkReqIsBlown_PreRepOffset_StackPointer,
  RtemsStackchkReqIsBlown_PreRepOffset_Pattern,
  RtemsStackchkReqIsBlown_PreRepOffset_InterruptStackPattern
};

static size_t RtemsStackchkReqIsBlown_GetRepresentativeCountOf(
  const RtemsStackchkReqIsBlown_Context *ctx,
  size_t                                 index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsStackchkReqIsBlown_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsStackchkReqIsBlown_GetRepresentative(
  const RtemsStackchkReqIsBlown_Context *ctx,
  size_t                                 index
)
{
  size_t count;

  count = RtemsStackchkReqIsBlown_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsStackchkReqIsBlown_GetRepresentativeCount(
  const RtemsStackchkReqIsBlown_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 4; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsStackchkReqIsBlown_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_Rep
RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_GetRepresentative(
  const RtemsStackchkReqIsBlown_Context *ctx
)
{
  return (
    RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_Rep
  ) RtemsStackchkReqIsBlown_GetRepresentative( ctx, 2 );
}

static RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_Rep
RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_GetRepresentative(
  const RtemsStackchkReqIsBlown_Context *ctx
)
{
  return (
    RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_Rep
  ) RtemsStackchkReqIsBlown_GetRepresentative( ctx, 3 );
}

static const uint8_t RtemsStackchkReqIsBlown_PreRepBase[] = { 0, 3, 7, 11 };

static const uint8_t RtemsStackchkReqIsBlown_PreRepStates[] = { 2, 3, 2, 2 };

static void RtemsStackchkReqIsBlown_ResetRepresentatives(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 15; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsStackchkReqIsBlown_MarkRepresentatives(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 4; ++i ) {
    size_t rep;

    rep = RtemsStackchkReqIsBlown_PreRepBase[ i ] +
          RtemsStackchkReqIsBlown_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsStackchkReqIsBlown_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsStackchkReqIsBlown_CheckRepresentatives(
  const RtemsStackchkReqIsBlown_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 4; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsStackchkReqIsBlown_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsStackchkReqIsBlown_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsStackchkReqIsBlown_PreRepStates[ i ]; ++state
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
            RtemsStackchkReqIsBlown_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef RtemsStackchkReqIsBlown_Context Context;

#define BUFFER_SIZE 512

/*
 * The pattern area sits at the end of the allocated stack area in the
 * direction of stack growth.  Each arrangement keeps the content of that end.
 */
static RTEMS_NO_INLINE void ArrangeStackPointer(
  Context       *ctx,
  Stack_Control *stack,
  uint8_t       *buffer,
  uintptr_t      stack_pointer
)
{
  void     *area;
  uintptr_t size;
  bool      copy;

  area = stack->area;
  size = stack->size;
  copy = false;

  switch ( ctx->position ) {
    case POSITION_BELOW:
#if CPU_STACK_GROWS_UP == TRUE
      area = (char *) stack->area + stack->size - BUFFER_SIZE;
#else
      area = buffer;
      copy = true;
#endif
      size = BUFFER_SIZE;
      T_assert_lt_uptr( stack_pointer, (uintptr_t) area );
      break;
    case POSITION_ABOVE:
#if CPU_STACK_GROWS_UP == TRUE
      area = buffer;
      copy = true;
#endif
      size = BUFFER_SIZE;
      T_assert_gt_uptr( stack_pointer, (uintptr_t) area + size );
      break;
    default:
      T_assert_ge_uptr( stack_pointer, (uintptr_t) area );
      T_assert_le_uptr( stack_pointer, (uintptr_t) area + size );
      break;
  }

  if ( copy ) {
#if CPU_STACK_GROWS_UP == TRUE
    memcpy(
      buffer,
      (const char *) stack->area + stack->size - BUFFER_SIZE,
      BUFFER_SIZE
    );
#else
    memcpy( buffer, stack->area, BUFFER_SIZE );
#endif
  }

  stack->area = area;
  stack->size = size;
}

static void SetInterruptStackBounds( Context *ctx, const Stack_Control *stack )
{
  if ( ctx->initialization ) {
    ctx->cpu_self->interrupt_stack_low = stack->area;
    ctx->cpu_self->interrupt_stack_high = (char *) stack->area + stack->size;
  }
}

static void RestoreInterruptStackBounds( Context *ctx )
{
  if ( ctx->initialization ) {
    ctx->cpu_self->interrupt_stack_low = ctx->interrupt_stack_low;
    ctx->cpu_self->interrupt_stack_high = ctx->interrupt_stack_high;
  }
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context *ctx;

  ctx = arg;
  ctx->fatal_source = source;
  ctx->fatal_code = code;
  ++ctx->fatal_counter;
  RestoreInterruptStackBounds( ctx );
  longjmp( ctx->before, 1 );
}

static void CallIsBlown( Context *const ctx )
{
  SetFatalHandler( Fatal, ctx );

  if ( setjmp( ctx->before ) == 0 ) {
    ctx->result = rtems_stack_checker_is_blown();
    ctx->returned = true;
    RestoreInterruptStackBounds( ctx );
  }

  SetFatalHandler( NULL, NULL );
}

static RTEMS_NO_INLINE void Act( Context *ctx, uint8_t *buffer )
{
  Stack_Control *stack;
  Stack_Control  interrupt_stack;
  uint8_t       *pattern;
  uint8_t       *interrupt_pattern;
  uintptr_t      stack_pointer;
  ISR_Level      level;

  stack_pointer = (uintptr_t) &stack_pointer;
  ctx->cpu_self = _Thread_Dispatch_disable();
  _ISR_Local_disable( level );

  if ( ctx->initialization ) {
    ctx->interrupt_stack_low = ctx->cpu_self->interrupt_stack_low;
    ctx->interrupt_stack_high = ctx->cpu_self->interrupt_stack_high;
    ctx->interrupt_stack.area = ctx->interrupt_stack_low;
    ctx->interrupt_stack.size = (uintptr_t) ctx->interrupt_stack_high -
                                (uintptr_t) ctx->interrupt_stack_low;
    ctx->stack = &ctx->interrupt_stack;
  } else {
    ctx->stack = &_Thread_Get_executing()->Start.Initial_stack;
  }

  stack = ctx->stack;
  ctx->saved_stack = *stack;
  ArrangeStackPointer( ctx, stack, buffer, stack_pointer );
  SetInterruptStackBounds( ctx, stack );

  pattern = (uint8_t *) _Stack_check_Get_pattern( stack ) + ctx->damage_offset;

  if ( ctx->damage ) {
    *pattern = (uint8_t) ~*pattern;
  }

  ctx->returned = false;
  ctx->result = true;
  ctx->fatal_counter = 0;

  interrupt_stack.area = ctx->cpu_self->interrupt_stack_low;
  interrupt_stack.size = (uintptr_t) ctx->cpu_self->interrupt_stack_high -
                         (uintptr_t) ctx->cpu_self->interrupt_stack_low;
  interrupt_pattern = (uint8_t *) _Stack_check_Get_pattern(
                        &interrupt_stack
                      ) +
                      ctx->interrupt_damage_offset;

  if ( ctx->damage_interrupt_stack ) {
    *interrupt_pattern = (uint8_t) ~*interrupt_pattern;
  }

  CallIsBlown( ctx );

  if ( ctx->damage_interrupt_stack ) {
    *interrupt_pattern = (uint8_t) ~*interrupt_pattern;
  }

  _ISR_Local_enable( level );
  _Thread_Dispatch_enable( ctx->cpu_self );

  if ( ctx->damage ) {
    *pattern = (uint8_t) ~*pattern;
  }

  *ctx->stack = ctx->saved_stack;
}

static void RtemsStackchkReqIsBlown_Pre_Context_Prepare(
  RtemsStackchkReqIsBlown_Context    *ctx,
  RtemsStackchkReqIsBlown_Pre_Context state
)
{
  switch ( state ) {
    case RtemsStackchkReqIsBlown_Pre_Context_Multitasking: {
      /*
       * While the multitasking started.
       */
      if ( !_System_state_Is_up( _System_state_Get() ) ) {
        ctx->Map.skip = true;
      }

      ctx->initialization = false;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_Context_Initialization: {
      /*
       * While the multitasking did not start.
       */
      if ( !_System_state_Is_before_multitasking( _System_state_Get() ) ) {
        ctx->Map.skip = true;
      }

      ctx->initialization = true;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_Context_NA:
      break;
  }
}

static void RtemsStackchkReqIsBlown_Pre_StackPointer_Prepare(
  RtemsStackchkReqIsBlown_Context         *ctx,
  RtemsStackchkReqIsBlown_Pre_StackPointer state
)
{
  switch ( state ) {
    case RtemsStackchkReqIsBlown_Pre_StackPointer_Below: {
      /*
       * While the stack pointer of the caller is below the begin address of
       * the allocated stack area of the checked stack.
       */
      ctx->position = POSITION_BELOW;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_StackPointer_Inside: {
      /*
       * While the stack pointer of the caller is inside the allocated stack
       * area of the checked stack.
       */
      ctx->position = POSITION_INSIDE;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_StackPointer_Above: {
      /*
       * While the stack pointer of the caller is above the end address of the
       * allocated stack area of the checked stack.
       */
      ctx->position = POSITION_ABOVE;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_StackPointer_NA:
      break;
  }
}

static void RtemsStackchkReqIsBlown_Pre_Pattern_Prepare(
  RtemsStackchkReqIsBlown_Context    *ctx,
  RtemsStackchkReqIsBlown_Pre_Pattern state
)
{
  switch ( state ) {
    case RtemsStackchkReqIsBlown_Pre_Pattern_Intact: {
      /*
       * While the stack checker pattern area of the stack of the executing
       * thread holds the pattern.
       */
      ctx->damage = false;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_Pattern_Damaged: {
      /*
       * While the stack checker pattern area of the stack of the executing
       * thread does not hold the pattern.
       */
      switch (
        RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_GetRepresentative( ctx )
      ) {
        case RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_First: {
          ctx->damage_offset = 0;
          break;
        }

        case RtemsStackchkReqIsBlown_Pre_Pattern_Damaged_Last: {
          ctx->damage_offset = STACK_CHECK_PATTERN_SIZE - 1;
          break;
        }
      }

      ctx->damage = true;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_Pattern_NA:
      break;
  }
}

static void RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Prepare(
  RtemsStackchkReqIsBlown_Context                  *ctx,
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern state
)
{
  switch ( state ) {
    case RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Intact: {
      /*
       * While the stack checker pattern area of the interrupt stack of the
       * current processor holds the pattern.
       */
      ctx->damage_interrupt_stack = false;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged: {
      /*
       * While the stack checker pattern area of the interrupt stack of the
       * current processor does not hold the pattern.
       */
      switch (
        RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_GetRepresentative(
          ctx
        )
      ) {
        case RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_First: {
          ctx->interrupt_damage_offset = 0;
          break;
        }

        case RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Damaged_Last: {
          ctx->interrupt_damage_offset = STACK_CHECK_PATTERN_SIZE - 1;
          break;
        }
      }

      ctx->damage_interrupt_stack = true;
      break;
    }

    case RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_NA:
      break;
  }
}

static void RtemsStackchkReqIsBlown_Post_Result_Check(
  RtemsStackchkReqIsBlown_Context    *ctx,
  RtemsStackchkReqIsBlown_Post_Result state
)
{
  switch ( state ) {
    case RtemsStackchkReqIsBlown_Post_Result_False: {
      /*
       * The return value of rtems_stack_checker_is_blown() shall be false.
       */
      T_true( ctx->returned );
      T_false( ctx->result );
      break;
    }

    case RtemsStackchkReqIsBlown_Post_Result_NoReturn: {
      /*
       * The rtems_stack_checker_is_blown() call shall not return to the
       * caller.
       */
      T_false( ctx->returned );
      break;
    }

    case RtemsStackchkReqIsBlown_Post_Result_NA:
      break;
  }
}

static void RtemsStackchkReqIsBlown_Post_Fatal_Check(
  RtemsStackchkReqIsBlown_Context   *ctx,
  RtemsStackchkReqIsBlown_Post_Fatal state
)
{
  switch ( state ) {
    case RtemsStackchkReqIsBlown_Post_Fatal_Nop: {
      /*
       * The rtems_stack_checker_is_blown() call shall raise no fatal error.
       */
      T_eq_u32( ctx->fatal_counter, 0 );
      break;
    }

    case RtemsStackchkReqIsBlown_Post_Fatal_StackChecker: {
      /*
       * The rtems_stack_checker_is_blown() call shall raise the fatal error
       * with the fatal source RTEMS_FATAL_SOURCE_STACK_CHECKER and the object
       * name of the executing thread as the fatal code.
       */
      T_eq_u32( ctx->fatal_counter, 1 );
      T_eq_int( ctx->fatal_source, RTEMS_FATAL_SOURCE_STACK_CHECKER );
      T_eq_ulong( ctx->fatal_code, ctx->name );
      break;
    }

    case RtemsStackchkReqIsBlown_Post_Fatal_StackCheckerInterrupt: {
      /*
       * The rtems_stack_checker_is_blown() call shall raise the fatal error
       * with the fatal source RTEMS_FATAL_SOURCE_STACK_CHECKER and the object
       * name built from the characters `INTR` as the fatal code.
       */
      T_eq_u32( ctx->fatal_counter, 1 );
      T_eq_int( ctx->fatal_source, RTEMS_FATAL_SOURCE_STACK_CHECKER );
      T_eq_ulong( ctx->fatal_code, rtems_build_name( 'I', 'N', 'T', 'R' ) );
      break;
    }

    case RtemsStackchkReqIsBlown_Post_Fatal_NA:
      break;
  }
}

static void RtemsStackchkReqIsBlown_Setup(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  ctx->name = _Thread_Get_executing()->Object.name.name_u32;
}

static void RtemsStackchkReqIsBlown_Setup_Wrap( void *arg )
{
  RtemsStackchkReqIsBlown_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsStackchkReqIsBlown_Setup( ctx );
}

static void RtemsStackchkReqIsBlown_Prepare(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  ctx->damage = false;
  ctx->damage_offset = 0;
  ctx->interrupt_damage_offset = 0;
}

static void RtemsStackchkReqIsBlown_Action(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  uint8_t buffer[ BUFFER_SIZE ] RTEMS_ALIGNED( CPU_STACK_ALIGNMENT );

  Act( ctx, buffer );
}

/* clang-format off */

static const RtemsStackchkReqIsBlown_Entry
RtemsStackchkReqIsBlown_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsStackchkReqIsBlown_Post_Result_NoReturn,
    RtemsStackchkReqIsBlown_Post_Fatal_StackChecker },
  { 1, 0, 0, 0, 0, RtemsStackchkReqIsBlown_Post_Result_NA,
    RtemsStackchkReqIsBlown_Post_Fatal_NA },
  { 0, 0, 0, 1, 0, RtemsStackchkReqIsBlown_Post_Result_NoReturn,
    RtemsStackchkReqIsBlown_Post_Fatal_StackCheckerInterrupt },
  { 0, 0, 0, 0, 0, RtemsStackchkReqIsBlown_Post_Result_False,
    RtemsStackchkReqIsBlown_Post_Fatal_Nop },
  { 0, 0, 0, 0, 0, RtemsStackchkReqIsBlown_Post_Result_NoReturn,
    RtemsStackchkReqIsBlown_Post_Fatal_StackCheckerInterrupt },
  { 0, 0, 0, 1, 0, RtemsStackchkReqIsBlown_Post_Result_False,
    RtemsStackchkReqIsBlown_Post_Fatal_Nop }
};

static const uint8_t
RtemsStackchkReqIsBlown_Map[] = {
  0, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 5, 2, 1, 1, 2, 2, 1, 1
};

/* clang-format on */

static size_t RtemsStackchkReqIsBlown_Scope( void *arg, char *buf, size_t n )
{
  RtemsStackchkReqIsBlown_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 4 ];
    size_t i;

    for ( i = 0; i < 4; ++i ) {
      pcd[ i ] = RtemsStackchkReqIsBlown_PreRepOffset[ i ]
                                                     [ ctx->Map.pcs[ i ] ] +
                 RtemsStackchkReqIsBlown_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsStackchkReqIsBlown_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsStackchkReqIsBlown_Fixture = {
  .setup = RtemsStackchkReqIsBlown_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsStackchkReqIsBlown_Scope,
  .initial_context = &RtemsStackchkReqIsBlown_Instance
};

static const uint8_t RtemsStackchkReqIsBlown_Weights[] = { 12, 4, 2, 1 };

static void RtemsStackchkReqIsBlown_Skip(
  RtemsStackchkReqIsBlown_Context *ctx,
  size_t                           index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = RtemsStackchkReqIsBlown_Pre_StackPointer_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = RtemsStackchkReqIsBlown_Pre_Pattern_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] =
        RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_NA - 1;
      break;
  }
}

static inline RtemsStackchkReqIsBlown_Entry RtemsStackchkReqIsBlown_PopEntry(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 4; ++i ) {
      index += RtemsStackchkReqIsBlown_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsStackchkReqIsBlown_Entries
    [ RtemsStackchkReqIsBlown_Map[ index ] ];
}

static void RtemsStackchkReqIsBlown_SetPreConditionStates(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Pattern_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsStackchkReqIsBlown_Pre_Pattern_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void RtemsStackchkReqIsBlown_TestVariant(
  RtemsStackchkReqIsBlown_Context *ctx
)
{
  RtemsStackchkReqIsBlown_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    RtemsStackchkReqIsBlown_Skip( ctx, 0 );
    return;
  }

  RtemsStackchkReqIsBlown_Pre_StackPointer_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsStackchkReqIsBlown_Pre_Pattern_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsStackchkReqIsBlown_Action( ctx );
  RtemsStackchkReqIsBlown_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  RtemsStackchkReqIsBlown_Post_Fatal_Check( ctx, ctx->Map.entry.Post_Fatal );
}

/**
 * @fn void T_case_body_RtemsStackchkReqIsBlown( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsStackchkReqIsBlown,
  &RtemsStackchkReqIsBlown_Fixture
)
{
  RtemsStackchkReqIsBlown_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  RtemsStackchkReqIsBlown_ResetRepresentatives( ctx );

  for (
    ctx->Map.pci[ 0 ] = RtemsStackchkReqIsBlown_Pre_Context_Multitasking;
    ctx->Map.pci[ 0 ] < RtemsStackchkReqIsBlown_Pre_Context_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsStackchkReqIsBlown_Pre_StackPointer_Below;
      ctx->Map.pci[ 1 ] < RtemsStackchkReqIsBlown_Pre_StackPointer_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsStackchkReqIsBlown_Pre_Pattern_Intact;
        ctx->Map.pci[ 2 ] < RtemsStackchkReqIsBlown_Pre_Pattern_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] =
            RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_Intact;
          ctx->Map.pci[ 3 ] <
          RtemsStackchkReqIsBlown_Pre_InterruptStackPattern_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsStackchkReqIsBlown_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsStackchkReqIsBlown_SetPreConditionStates( ctx );

          for (
            ctx->Map.rep = 0;
            ctx->Map.rep <
            RtemsStackchkReqIsBlown_GetRepresentativeCount( ctx );
            ++ctx->Map.rep
          ) {
            ctx->Map.skip_rep = false;
            RtemsStackchkReqIsBlown_Prepare( ctx );
            RtemsStackchkReqIsBlown_TestVariant( ctx );

            if ( ctx->Map.skip ) {
              break;
            }

            RtemsStackchkReqIsBlown_MarkRepresentatives( ctx );
          }
        }
      }
    }
  }

  RtemsStackchkReqIsBlown_CheckRepresentatives( ctx );
}

/** @} */
