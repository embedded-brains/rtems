/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqOnce
 */

/*
 * Copyright (C) 2026 OHB System AG
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

#include <errno.h>
#include <pthread.h>
#include <rtems.h>
#include <string.h>
#include <rtems/score/statesimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqOnce spec:/c/pthread/req/once
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqOnce_Pre_Control_Obj,
  CPthreadReqOnce_Pre_Control_Null,
  CPthreadReqOnce_Pre_Control_NA
} CPthreadReqOnce_Pre_Control;

typedef enum {
  CPthreadReqOnce_Pre_Routine_Valid,
  CPthreadReqOnce_Pre_Routine_Null,
  CPthreadReqOnce_Pre_Routine_NA
} CPthreadReqOnce_Pre_Routine;

typedef enum {
  CPthreadReqOnce_Pre_State_Pending,
  CPthreadReqOnce_Pre_State_Running,
  CPthreadReqOnce_Pre_State_Done,
  CPthreadReqOnce_Pre_State_NA
} CPthreadReqOnce_Pre_State;

typedef enum {
  CPthreadReqOnce_Pre_Request_None,
  CPthreadReqOnce_Pre_Request_Pending,
  CPthreadReqOnce_Pre_Request_NA
} CPthreadReqOnce_Pre_Request;

typedef enum {
  CPthreadReqOnce_Pre_Cancel_None,
  CPthreadReqOnce_Pre_Cancel_Point,
  CPthreadReqOnce_Pre_Cancel_NA
} CPthreadReqOnce_Pre_Cancel;

typedef enum {
  CPthreadReqOnce_Pre_Waiters_None,
  CPthreadReqOnce_Pre_Waiters_Some,
  CPthreadReqOnce_Pre_Waiters_NA
} CPthreadReqOnce_Pre_Waiters;

typedef enum {
  CPthreadReqOnce_Post_Status_Ok,
  CPthreadReqOnce_Post_Status_Inval,
  CPthreadReqOnce_Post_Status_NA
} CPthreadReqOnce_Post_Status;

typedef enum {
  CPthreadReqOnce_Post_Errno_Nop,
  CPthreadReqOnce_Post_Errno_NA
} CPthreadReqOnce_Post_Errno;

typedef enum {
  CPthreadReqOnce_Post_Routine_Once,
  CPthreadReqOnce_Post_Routine_None,
  CPthreadReqOnce_Post_Routine_NA
} CPthreadReqOnce_Post_Routine;

typedef enum {
  CPthreadReqOnce_Post_Completed_Yes,
  CPthreadReqOnce_Post_Completed_NA
} CPthreadReqOnce_Post_Completed;

typedef enum {
  CPthreadReqOnce_Post_Effects_Visible,
  CPthreadReqOnce_Post_Effects_NA
} CPthreadReqOnce_Post_Effects;

typedef enum {
  CPthreadReqOnce_Post_Control_Done,
  CPthreadReqOnce_Post_Control_Nop,
  CPthreadReqOnce_Post_Control_NA
} CPthreadReqOnce_Post_Control;

typedef enum {
  CPthreadReqOnce_Post_Caller_Returned,
  CPthreadReqOnce_Post_Caller_NA
} CPthreadReqOnce_Post_Caller;

typedef enum {
  CPthreadReqOnce_Post_Request_Kept,
  CPthreadReqOnce_Post_Request_NA
} CPthreadReqOnce_Post_Request;

typedef enum {
  CPthreadReqOnce_Post_Waiters_Released,
  CPthreadReqOnce_Post_Waiters_NA
} CPthreadReqOnce_Post_Waiters;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Control_NA : 1;
  uint32_t Pre_Routine_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Pre_Request_NA : 1;
  uint32_t Pre_Cancel_NA : 1;
  uint32_t Pre_Waiters_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Errno : 1;
  uint32_t Post_Routine : 2;
  uint32_t Post_Completed : 1;
  uint32_t Post_Effects : 1;
  uint32_t Post_Control : 2;
  uint32_t Post_Caller : 1;
  uint32_t Post_Request : 1;
  uint32_t Post_Waiters : 1;
} CPthreadReqOnce_Entry;

#define WAITER_COUNT 2

#define DATA_DONE 0x600dd00d

typedef enum {
  CALLS_ROUTINE,
  CALLS_OTHER,
  CALLS_PROBE,
  CALLS_WAITER,
  CALLS_COUNT
} Calls;

typedef enum { STATE_PENDING, STATE_RUNNING, STATE_DONE } State;

/**
 * @brief Test context for spec:/c/pthread/req/once test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `once_control` parameter value.
   */
  pthread_once_t *once_control;

  /**
   * @brief This member specifies the `init_routine` parameter value.
   */
  void ( *init_routine )( void );

  /**
   * @brief This member contains the return value of the pthread_once() call.
   */
  int retval;

  /**
   * @brief This member contains the value of errno after the pthread_once()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member provides the once-control object.
   */
  pthread_once_t once_obj;

  /**
   * @brief This member specifies the state of the once-control object.
   */
  State state;

  /**
   * @brief This member is true, if a cancel request shall be pending for the
   *   calling thread.
   */
  bool request;

  /**
   * @brief This member is true, if the initialization routine shall reach a
   *   cancellation point with a pending cancel request.
   */
  bool cancel;

  /**
   * @brief This member is true, if other threads shall call the function
   *   during the execution of the initialization routine.
   */
  bool waiters;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the identifier of the caller task.
   */
  rtems_id caller_id;

  /**
   * @brief This member contains the identifier of the other task, which
   *   executes the initialization routine while the caller task calls the
   *   function.
   */
  rtems_id other_id;

  /**
   * @brief This member contains the index of the processor of the caller task.
   */
  uint32_t caller_cpu;

  /**
   * @brief This member contains the identifier of the task which executed the
   *   initialization routine of the caller task.
   */
  rtems_id routine_id;

  /**
   * @brief This member contains the counts of the calls of the initialization
   *   routines, indexed by the routine.
   */
  uint32_t calls[ CALLS_COUNT ];

  /**
   * @brief This member is set by the initialization routines.
   */
  uint32_t data;

  /**
   * @brief This member is true, if an initialization routine completed.
   */
  bool routine_completed;

  /**
   * @brief This member contains the value of the data member which the caller
   *   task read after the call.
   */
  uint32_t seen_data;

  /**
   * @brief This member is true, if the caller task read the completion state
   *   of the initialization routine as true after the call.
   */
  bool seen_completed;

  /**
   * @brief This member is true, if the caller task continued after a call of
   *   pthread_testcancel() which it made with a pending request.
   */
  bool survived;

  /**
   * @brief This member is true, if the call returned to the caller task.
   */
  bool returned;

  /**
   * @brief This member is set to one, when the caller task ended its call.
   */
  Atomic_Uint caller_done;

  /**
   * @brief This member contains the exit value of the caller task.
   */
  void *caller_exit;

  /**
   * @brief This member contains the exit value of the other task.
   */
  void *other_exit;

  /**
   * @brief This member contains the identifiers of the waiter tasks.
   */
  rtems_id waiter_id[ WAITER_COUNT ];

  /**
   * @brief This member contains the count of waiter tasks whose call returned.
   */
  uint32_t waiters_returned;

  /**
   * @brief This member contains the count of waiter tasks whose call returned
   *   a value other than zero.
   */
  uint32_t waiters_failed;

  /**
   * @brief This member contains the count of waiter tasks whose call returned
   *   after the initialization routine completed.
   */
  uint32_t waiters_after_routine;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    CPthreadReqOnce_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqOnce_Context;

static CPthreadReqOnce_Context CPthreadReqOnce_Instance;

static const char *const CPthreadReqOnce_PreDesc_Control[] =
  { "Obj", "Null", "NA" };

static const char *const CPthreadReqOnce_PreDesc_Routine[] =
  { "Valid", "Null", "NA" };

static const char *const CPthreadReqOnce_PreDesc_State[] =
  { "Pending", "Running", "Done", "NA" };

static const char *const CPthreadReqOnce_PreDesc_Request[] =
  { "None", "Pending", "NA" };

static const char *const CPthreadReqOnce_PreDesc_Cancel[] =
  { "None", "Point", "NA" };

static const char *const CPthreadReqOnce_PreDesc_Waiters[] =
  { "None", "Some", "NA" };

static const char *const *const CPthreadReqOnce_PreDesc[] = {
  CPthreadReqOnce_PreDesc_Control,
  CPthreadReqOnce_PreDesc_Routine,
  CPthreadReqOnce_PreDesc_State,
  CPthreadReqOnce_PreDesc_Request,
  CPthreadReqOnce_PreDesc_Cancel,
  CPthreadReqOnce_PreDesc_Waiters,
  NULL
};

typedef CPthreadReqOnce_Context Context;

#define ERRNO_PATTERN 1234

static const pthread_once_t once_init = PTHREAD_ONCE_INIT;

static int caller_exit_object;

static int other_exit_object;

static void Routine( void )
{
  Context *ctx;
  size_t   i;

  ctx = &CPthreadReqOnce_Instance;
  ++ctx->calls[ CALLS_ROUTINE ];
  ctx->routine_id = rtems_task_self();

  for ( i = 0; ctx->waiters && i < WAITER_COUNT; ++i ) {
    /*
     * The waiter has a higher priority.  It runs now, calls the function for
     * the once-control object, whose routine this thread executes, and waits.
     * This thread continues, once the waiter is blocked.
     */
    SendEvents( ctx->waiter_id[ i ], RTEMS_EVENT_0 );
  }

  if ( ctx->cancel && ctx->state == STATE_PENDING ) {
    T_eq_int( pthread_cancel( pthread_self() ), 0 );
    pthread_testcancel();
  }

  ctx->data = DATA_DONE;
  ctx->routine_completed = true;
}

static void OtherRoutine( void )
{
  Context *ctx;

  ctx = &CPthreadReqOnce_Instance;
  ++ctx->calls[ CALLS_OTHER ];

  /* The caller task calls the function, while this routine waits. */
  (void) ReceiveAnyEvents();

  if ( ctx->cancel ) {
    T_eq_int( pthread_cancel( pthread_self() ), 0 );
    pthread_testcancel();
  }

  ctx->data = DATA_DONE;
  ctx->routine_completed = true;
}

static void ProbeRoutine( void )
{
  ++CPthreadReqOnce_Instance.calls[ CALLS_PROBE ];
}

static void WaiterRoutine( void )
{
  ++CPthreadReqOnce_Instance.calls[ CALLS_WAITER ];
}

static void Waiter( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;
  (void) ReceiveAnyEvents();
  eno = pthread_once( &ctx->once_obj, WaiterRoutine );

  if ( eno != 0 ) {
    ++ctx->waiters_failed;
  }

  if ( ctx->routine_completed ) {
    ++ctx->waiters_after_routine;
  }

  ++ctx->waiters_returned;
  SuspendSelf();
}

static void Other( rtems_task_argument arg )
{
  Context *ctx;
  int      eno;

  ctx = (Context *) arg;
  SetCancelability( PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_DEFERRED );
  eno = pthread_once( &ctx->once_obj, OtherRoutine );
  T_eq_int( eno, 0 );

  if ( ctx->cancel ) {
    pthread_testcancel();
  }

  pthread_exit( &other_exit_object );
}

static void Caller( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx->request ) {
    /* The request stays pending, since the cancel state is disabled. */
    DisableCancelability( NULL, NULL );
    T_eq_int( pthread_cancel( pthread_self() ), 0 );
  }

  SetCancelability( PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_DEFERRED );
  T_eq_u32( rtems_scheduler_get_processor(), ctx->caller_cpu );
  errno = ERRNO_PATTERN;
  ctx->retval = pthread_once( ctx->once_control, ctx->init_routine );
  ctx->errno_value = errno;
  ctx->seen_data = ctx->data;
  ctx->seen_completed = ctx->routine_completed;
  ctx->returned = true;
  SetFlag( &ctx->caller_done, 1 );

  if ( ctx->request || ( ctx->cancel && ctx->state == STATE_PENDING ) ) {
    /* The request is still pending, so the thread ends here. */
    pthread_testcancel();
    ctx->survived = true;
  }

  pthread_exit( &caller_exit_object );
}

static void Prepare( Context *ctx )
{
  int    eno;
  size_t i;

  ctx->once_obj = once_init;

  switch ( ctx->state ) {
    case STATE_DONE:
      eno = pthread_once( &ctx->once_obj, Routine );
      T_eq_int( eno, 0 );
      T_eq_u32( ctx->calls[ CALLS_ROUTINE ], 1 );
      ctx->calls[ CALLS_ROUTINE ] = 0;
      ctx->routine_id = RTEMS_ID_NONE;
      break;
    case STATE_RUNNING:
      ctx->other_id = CreateTask( "OTHR", PRIO_HIGH );
      SetAffinityOne( ctx->other_id, 0 );

      /*
       * The other task has a higher priority than the runner on the same
       * processor.  It runs now and waits in its initialization routine.
       */
      StartTask( ctx->other_id, Other, ctx );
      T_eq_u32( ctx->calls[ CALLS_OTHER ], 1 );
      break;
    default:
      break;
  }

  if ( ctx->waiters ) {
    for ( i = 0; i < WAITER_COUNT; ++i ) {
      ctx->waiter_id[ i ] = CreateTask( "WAIT", PRIO_VERY_HIGH );
      SetAffinityOne( ctx->waiter_id[ i ], 0 );
      StartTask( ctx->waiter_id[ i ], Waiter, ctx );
    }
  }

  if (
    ctx->state != STATE_PENDING && rtems_scheduler_get_processor_maximum() > 1
  ) {
    ctx->caller_cpu = 1;
  }
}

static void CheckControl( Context *ctx, uint32_t probe_calls )
{
  int eno;

  eno = pthread_once( &ctx->once_obj, ProbeRoutine );
  T_eq_int( eno, 0 );
  T_eq_u32( ctx->calls[ CALLS_PROBE ], probe_calls );
}

static void CPthreadReqOnce_Pre_Control_Prepare(
  CPthreadReqOnce_Context    *ctx,
  CPthreadReqOnce_Pre_Control state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_Control_Obj: {
      /*
       * While the `once_control` parameter references an object of type
       * pthread_once_t which was initialized by PTHREAD_ONCE_INIT.
       */
      ctx->once_control = &ctx->once_obj;
      break;
    }

    case CPthreadReqOnce_Pre_Control_Null: {
      /*
       * While the `once_control` parameter is NULL.
       */
      ctx->once_control = NULL;
      break;
    }

    case CPthreadReqOnce_Pre_Control_NA:
      break;
  }
}

static void CPthreadReqOnce_Pre_Routine_Prepare(
  CPthreadReqOnce_Context    *ctx,
  CPthreadReqOnce_Pre_Routine state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_Routine_Valid: {
      /*
       * While the `init_routine` parameter references an initialization
       * routine.
       */
      ctx->init_routine = Routine;
      break;
    }

    case CPthreadReqOnce_Pre_Routine_Null: {
      /*
       * While the `init_routine` parameter is NULL.
       */
      ctx->init_routine = NULL;
      break;
    }

    case CPthreadReqOnce_Pre_Routine_NA:
      break;
  }
}

static void CPthreadReqOnce_Pre_State_Prepare(
  CPthreadReqOnce_Context  *ctx,
  CPthreadReqOnce_Pre_State state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_State_Pending: {
      /*
       * While the initialization routine associated with the once-control
       * object referenced by the `once_control` parameter has not been called.
       */
      ctx->state = STATE_PENDING;
      break;
    }

    case CPthreadReqOnce_Pre_State_Running: {
      /*
       * While another thread executes the initialization routine associated
       * with the once-control object referenced by the `once_control`
       * parameter.
       */
      ctx->state = STATE_RUNNING;
      break;
    }

    case CPthreadReqOnce_Pre_State_Done: {
      /*
       * While the initialization routine associated with the once-control
       * object referenced by the `once_control` parameter has completed.
       */
      ctx->state = STATE_DONE;
      break;
    }

    case CPthreadReqOnce_Pre_State_NA:
      break;
  }
}

static void CPthreadReqOnce_Pre_Request_Prepare(
  CPthreadReqOnce_Context    *ctx,
  CPthreadReqOnce_Pre_Request state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_Request_None: {
      /*
       * While no thread cancellation request is pending for the calling
       * thread.
       */
      ctx->request = false;
      break;
    }

    case CPthreadReqOnce_Pre_Request_Pending: {
      /*
       * While a thread cancellation request is pending for the calling thread,
       * while the thread cancel state attribute of the calling thread is
       * PTHREAD_CANCEL_ENABLE and its thread cancel type attribute is
       * PTHREAD_CANCEL_DEFERRED.
       */
      ctx->request = true;
      break;
    }

    case CPthreadReqOnce_Pre_Request_NA:
      break;
  }
}

static void CPthreadReqOnce_Pre_Cancel_Prepare(
  CPthreadReqOnce_Context   *ctx,
  CPthreadReqOnce_Pre_Cancel state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_Cancel_None: {
      /*
       * While the initialization routine does not call pthread_testcancel().
       */
      ctx->cancel = false;
      break;
    }

    case CPthreadReqOnce_Pre_Cancel_Point: {
      /*
       * While the initialization routine calls pthread_testcancel(), while a
       * thread cancellation request is pending for the thread which executes
       * it, while the thread cancel state attribute of this thread is
       * PTHREAD_CANCEL_ENABLE and its thread cancel type attribute is
       * PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel = true;
      break;
    }

    case CPthreadReqOnce_Pre_Cancel_NA:
      break;
  }
}

static void CPthreadReqOnce_Pre_Waiters_Prepare(
  CPthreadReqOnce_Context    *ctx,
  CPthreadReqOnce_Pre_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_Waiters_None: {
      /*
       * While no other thread calls pthread_once() for the once-control object
       * referenced by the `once_control` parameter during the execution of the
       * initialization routine.
       */
      ctx->waiters = false;
      break;
    }

    case CPthreadReqOnce_Pre_Waiters_Some: {
      /*
       * While other threads call pthread_once() for the once-control object
       * referenced by the `once_control` parameter during the execution of the
       * initialization routine.
       */
      ctx->waiters = true;
      break;
    }

    case CPthreadReqOnce_Pre_Waiters_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Status_Check(
  CPthreadReqOnce_Context    *ctx,
  CPthreadReqOnce_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Status_Ok: {
      /*
       * The return value of pthread_once() shall be zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqOnce_Post_Status_Inval: {
      /*
       * The return value of pthread_once() shall be EINVAL.
       */
      T_eq_int( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqOnce_Post_Status_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Errno_Check(
  CPthreadReqOnce_Context   *ctx,
  CPthreadReqOnce_Post_Errno state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Errno_Nop: {
      /*
       * The value of errno shall not be changed by the pthread_once() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_PATTERN );
      break;
    }

    case CPthreadReqOnce_Post_Errno_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Routine_Check(
  CPthreadReqOnce_Context     *ctx,
  CPthreadReqOnce_Post_Routine state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Routine_Once: {
      /*
       * The initialization routine referenced by the `init_routine` parameter
       * shall be called once by the calling thread, before the pthread_once()
       * call returns.
       */
      T_eq_u32( ctx->calls[ CALLS_ROUTINE ], 1 );
      T_eq_u32( ctx->routine_id, ctx->caller_id );
      break;
    }

    case CPthreadReqOnce_Post_Routine_None: {
      /*
       * The initialization routine referenced by the `init_routine` parameter
       * shall not be called.
       */
      T_eq_u32( ctx->calls[ CALLS_ROUTINE ], 0 );
      break;
    }

    case CPthreadReqOnce_Post_Routine_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Completed_Check(
  CPthreadReqOnce_Context       *ctx,
  CPthreadReqOnce_Post_Completed state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Completed_Yes: {
      /*
       * When the pthread_once() call returns, the initialization routine
       * associated with the once-control object referenced by the
       * `once_control` parameter shall have completed.
       */
      T_true( ctx->seen_completed );
      break;
    }

    case CPthreadReqOnce_Post_Completed_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Effects_Check(
  CPthreadReqOnce_Context     *ctx,
  CPthreadReqOnce_Post_Effects state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Effects_Visible: {
      /*
       * When the pthread_once() call returns, the effects of the
       * initialization routine shall be visible to the calling thread.
       */
      T_eq_u32( ctx->seen_data, DATA_DONE );
      break;
    }

    case CPthreadReqOnce_Post_Effects_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Control_Check(
  CPthreadReqOnce_Context     *ctx,
  CPthreadReqOnce_Post_Control state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Control_Done: {
      /*
       * A later pthread_once() call for the once-control object referenced by
       * the `once_control` parameter shall not call its initialization
       * routine.
       */
      CheckControl( ctx, 0 );
      break;
    }

    case CPthreadReqOnce_Post_Control_Nop: {
      /*
       * A later pthread_once() call for the once-control object referenced by
       * the `once_control` parameter shall call its initialization routine
       * once, as if pthread_once() was never called for this object.
       */
      CheckControl( ctx, 1 );
      break;
    }

    case CPthreadReqOnce_Post_Control_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Caller_Check(
  CPthreadReqOnce_Context    *ctx,
  CPthreadReqOnce_Post_Caller state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Caller_Returned: {
      /*
       * The pthread_once() call shall return to the calling thread.
       */
      T_true( ctx->returned );
      break;
    }

    case CPthreadReqOnce_Post_Caller_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Request_Check(
  CPthreadReqOnce_Context     *ctx,
  CPthreadReqOnce_Post_Request state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Request_Kept: {
      /*
       * The thread cancellation request shall stay pending for the thread to
       * which it was made, so that the thread is terminated at its next call
       * of pthread_testcancel() after the pthread_once() call returned.
       */
      if ( ctx->request || ( ctx->cancel && ctx->state == STATE_PENDING ) ) {
        T_false( ctx->survived );
        T_eq_ptr( ctx->caller_exit, PTHREAD_CANCELED );
      }

      if ( ctx->cancel && ctx->state == STATE_RUNNING ) {
        T_eq_ptr( ctx->other_exit, PTHREAD_CANCELED );
      }
      break;
    }

    case CPthreadReqOnce_Post_Request_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Waiters_Check(
  CPthreadReqOnce_Context     *ctx,
  CPthreadReqOnce_Post_Waiters state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Waiters_Released: {
      /*
       * Each other thread which calls pthread_once() for the once-control
       * object referenced by the `once_control` parameter while the
       * initialization routine executes shall return zero from its call after
       * the initialization routine completed, without a call of its own
       * initialization routine.
       */
      T_eq_u32( ctx->waiters_returned, WAITER_COUNT );
      T_eq_u32( ctx->waiters_after_routine, WAITER_COUNT );
      T_eq_u32( ctx->waiters_failed, 0 );
      T_eq_u32( ctx->calls[ CALLS_WAITER ], 0 );
      break;
    }

    case CPthreadReqOnce_Post_Waiters_NA:
      break;
  }
}

static void CPthreadReqOnce_Setup( CPthreadReqOnce_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );
  ctx->runner_id = rtems_task_self();
}

static void CPthreadReqOnce_Setup_Wrap( void *arg )
{
  CPthreadReqOnce_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqOnce_Setup( ctx );
}

static void CPthreadReqOnce_Teardown( void )
{
  SetSelfAffinityAll();
  RestoreRunnerPriority();
}

static void CPthreadReqOnce_Teardown_Wrap( void *arg )
{
  CPthreadReqOnce_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqOnce_Teardown();
}

static void CPthreadReqOnce_Prepare( CPthreadReqOnce_Context *ctx )
{
  ctx->once_control = NULL;
  ctx->init_routine = NULL;
  ctx->retval = -1;
  ctx->errno_value = -1;
  ctx->state = STATE_PENDING;
  ctx->request = false;
  ctx->cancel = false;
  ctx->waiters = false;
  ctx->caller_id = RTEMS_ID_NONE;
  ctx->other_id = RTEMS_ID_NONE;
  ctx->caller_cpu = 0;
  ctx->routine_id = RTEMS_ID_NONE;
  memset( &ctx->calls, 0, sizeof( ctx->calls ) );
  ctx->data = 0;
  ctx->routine_completed = false;
  ctx->seen_data = 0;
  ctx->seen_completed = false;
  ctx->returned = false;
  ctx->survived = false;
  SetFlag( &ctx->caller_done, 0 );
  ctx->caller_exit = NULL;
  ctx->other_exit = NULL;
  memset( &ctx->waiter_id, 0, sizeof( ctx->waiter_id ) );
  ctx->waiters_returned = 0;
  ctx->waiters_failed = 0;
  ctx->waiters_after_routine = 0;
}

static void CPthreadReqOnce_Action( CPthreadReqOnce_Context *ctx )
{
  Prepare( ctx );
  ctx->caller_id = CreateTask( "CALL", PRIO_HIGH );

  if ( ctx->caller_cpu != 0 ) {
    /* The processor with the index one belongs to the scheduler B. */
    SetScheduler( ctx->caller_id, SCHEDULER_B_ID, PRIO_HIGH );
  }

  SetAffinityOne( ctx->caller_id, ctx->caller_cpu );
  StartTask( ctx->caller_id, Caller, ctx );

  if ( ctx->state == STATE_RUNNING ) {
    if ( ctx->once_control != NULL && ctx->init_routine != NULL ) {
      T_true( WaitForBlockedState(
        ctx->caller_id,
        STATES_WAITING_FOR_CONDITION_VARIABLE,
        &ctx->caller_done
      ) );
      T_eq_uint( GetFlag( &ctx->caller_done ), 0 );
    }

    SendEvents( ctx->other_id, RTEMS_EVENT_0 );
    T_eq_int( pthread_join( (pthread_t) ctx->other_id, &ctx->other_exit ), 0 );
  }

  T_eq_int( pthread_join( (pthread_t) ctx->caller_id, &ctx->caller_exit ), 0 );
}

static void CPthreadReqOnce_Cleanup( CPthreadReqOnce_Context *ctx )
{
  if ( ctx->waiters ) {
    size_t i;

    for ( i = 0; i < WAITER_COUNT; ++i ) {
      DeleteTask( ctx->waiter_id[ i ] );
    }
  }
}

/* clang-format off */

static const CPthreadReqOnce_Entry
CPthreadReqOnce_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, CPthreadReqOnce_Post_Status_NA,
    CPthreadReqOnce_Post_Errno_NA, CPthreadReqOnce_Post_Routine_NA,
    CPthreadReqOnce_Post_Completed_NA, CPthreadReqOnce_Post_Effects_NA,
    CPthreadReqOnce_Post_Control_NA, CPthreadReqOnce_Post_Caller_NA,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 0, 1, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_Once,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_Kept, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 0, 1, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_Kept, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 1, 1, 1, CPthreadReqOnce_Post_Status_Inval,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_NA, CPthreadReqOnce_Post_Effects_NA,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 1, 1, 1, 1, CPthreadReqOnce_Post_Status_Inval,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_NA, CPthreadReqOnce_Post_Effects_NA,
    CPthreadReqOnce_Post_Control_NA, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_Once,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 0, 0, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_Once,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_Released },
  { 0, 0, 0, 0, 0, 0, 1, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 1, 1, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 0, 1, 1, CPthreadReqOnce_Post_Status_Ok,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_Yes, CPthreadReqOnce_Post_Effects_Visible,
    CPthreadReqOnce_Post_Control_Done, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_Kept, CPthreadReqOnce_Post_Waiters_NA },
  { 0, 0, 0, 0, 1, 1, 1, CPthreadReqOnce_Post_Status_Inval,
    CPthreadReqOnce_Post_Errno_Nop, CPthreadReqOnce_Post_Routine_None,
    CPthreadReqOnce_Post_Completed_NA, CPthreadReqOnce_Post_Effects_NA,
    CPthreadReqOnce_Post_Control_Nop, CPthreadReqOnce_Post_Caller_Returned,
    CPthreadReqOnce_Post_Request_NA, CPthreadReqOnce_Post_Waiters_NA }
};

static const uint8_t
CPthreadReqOnce_Map[] = {
  5, 6, 1, 0, 1, 0, 1, 0, 7, 0, 2, 0, 2, 0, 2, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10,
  0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqOnce_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqOnce_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqOnce_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqOnce_Fixture = {
  .setup = CPthreadReqOnce_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqOnce_Teardown_Wrap,
  .scope = CPthreadReqOnce_Scope,
  .initial_context = &CPthreadReqOnce_Instance
};

static inline CPthreadReqOnce_Entry CPthreadReqOnce_PopEntry(
  CPthreadReqOnce_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqOnce_Entries[ CPthreadReqOnce_Map[ index ] ];
}

static void CPthreadReqOnce_SetPreConditionStates(
  CPthreadReqOnce_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqOnce_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Request_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqOnce_Pre_Request_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Cancel_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqOnce_Pre_Cancel_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Waiters_NA ) {
    ctx->Map.pcs[ 5 ] = CPthreadReqOnce_Pre_Waiters_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void CPthreadReqOnce_TestVariant( CPthreadReqOnce_Context *ctx )
{
  CPthreadReqOnce_Pre_Control_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqOnce_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqOnce_Pre_State_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqOnce_Pre_Request_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqOnce_Pre_Cancel_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqOnce_Pre_Waiters_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqOnce_Action( ctx );
  CPthreadReqOnce_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqOnce_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CPthreadReqOnce_Post_Routine_Check( ctx, ctx->Map.entry.Post_Routine );
  CPthreadReqOnce_Post_Completed_Check( ctx, ctx->Map.entry.Post_Completed );
  CPthreadReqOnce_Post_Effects_Check( ctx, ctx->Map.entry.Post_Effects );
  CPthreadReqOnce_Post_Control_Check( ctx, ctx->Map.entry.Post_Control );
  CPthreadReqOnce_Post_Caller_Check( ctx, ctx->Map.entry.Post_Caller );
  CPthreadReqOnce_Post_Request_Check( ctx, ctx->Map.entry.Post_Request );
  CPthreadReqOnce_Post_Waiters_Check( ctx, ctx->Map.entry.Post_Waiters );
}

/**
 * @fn void T_case_body_CPthreadReqOnce( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqOnce, &CPthreadReqOnce_Fixture )
{
  CPthreadReqOnce_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqOnce_Pre_Control_Obj;
    ctx->Map.pci[ 0 ] < CPthreadReqOnce_Pre_Control_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqOnce_Pre_Routine_Valid;
      ctx->Map.pci[ 1 ] < CPthreadReqOnce_Pre_Routine_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqOnce_Pre_State_Pending;
        ctx->Map.pci[ 2 ] < CPthreadReqOnce_Pre_State_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqOnce_Pre_Request_None;
          ctx->Map.pci[ 3 ] < CPthreadReqOnce_Pre_Request_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqOnce_Pre_Cancel_None;
            ctx->Map.pci[ 4 ] < CPthreadReqOnce_Pre_Cancel_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CPthreadReqOnce_Pre_Waiters_None;
              ctx->Map.pci[ 5 ] < CPthreadReqOnce_Pre_Waiters_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = CPthreadReqOnce_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              CPthreadReqOnce_SetPreConditionStates( ctx );
              CPthreadReqOnce_Prepare( ctx );
              CPthreadReqOnce_TestVariant( ctx );
              CPthreadReqOnce_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
