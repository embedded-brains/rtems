/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetnameNp
 */

/*
 * Copyright (C) 2025 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSetnameNp spec:/c/pthread/req/setname-np
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSetnameNp_Pre_Thread_NoObj,
  CPthreadReqSetnameNp_Pre_Thread_Valid,
  CPthreadReqSetnameNp_Pre_Thread_NA
} CPthreadReqSetnameNp_Pre_Thread;

typedef enum {
  CPthreadReqSetnameNp_Pre_Length_Fits,
  CPthreadReqSetnameNp_Pre_Length_TooLong,
  CPthreadReqSetnameNp_Pre_Length_NA
} CPthreadReqSetnameNp_Pre_Length;

typedef enum {
  CPthreadReqSetnameNp_Post_Status_Zero,
  CPthreadReqSetnameNp_Post_Status_ESRCH,
  CPthreadReqSetnameNp_Post_Status_ERANGE,
  CPthreadReqSetnameNp_Post_Status_NA
} CPthreadReqSetnameNp_Post_Status;

typedef enum {
  CPthreadReqSetnameNp_Post_Name_Partial,
  CPthreadReqSetnameNp_Post_Name_Full,
  CPthreadReqSetnameNp_Post_Name_NA
} CPthreadReqSetnameNp_Post_Name;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Thread_NA : 1;
  uint8_t Pre_Length_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Name : 2;
} CPthreadReqSetnameNp_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/setname-np test case.
 */
typedef struct {
  /**
   * @brief This member provides a task with a name.
   */
  rtems_id worker_id;

  /**
   * @brief This member provides a buffer for the thread name.
   */
  char name_buf[ 16 ];;

  /**
   * @brief This member provides the ``thread`` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member provides the ``name`` parameter value.
   */
  const char *name;

  /**
   * @brief This member contains the returned status.
   */
  int status;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    CPthreadReqSetnameNp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetnameNp_Context;

static CPthreadReqSetnameNp_Context
  CPthreadReqSetnameNp_Instance;

static const char * const CPthreadReqSetnameNp_PreDesc_Thread[] = {
  "NoObj",
  "Valid",
  "NA"
};

static const char * const CPthreadReqSetnameNp_PreDesc_Length[] = {
  "Fits",
  "TooLong",
  "NA"
};

static const char * const * const CPthreadReqSetnameNp_PreDesc[] = {
  CPthreadReqSetnameNp_PreDesc_Thread,
  CPthreadReqSetnameNp_PreDesc_Length,
  NULL
};

static const char fitting_name[] = "foobar";

static const char too_long_name[] = "0123456789abcdef0123456789abcdef";

static void CPthreadReqSetnameNp_Pre_Thread_Prepare(
  CPthreadReqSetnameNp_Context   *ctx,
  CPthreadReqSetnameNp_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqSetnameNp_Pre_Thread_NoObj: {
      /*
       * While the ``thread`` parameter is not associated with a thread.
       */
      ctx->thread = 0xffffffff;
      break;
    }

    case CPthreadReqSetnameNp_Pre_Thread_Valid: {
      /*
       * While the ``thread`` parameter is associated with a thread.
       */
      ctx->thread = ctx->worker_id;
      break;
    }

    case CPthreadReqSetnameNp_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqSetnameNp_Pre_Length_Prepare(
  CPthreadReqSetnameNp_Context   *ctx,
  CPthreadReqSetnameNp_Pre_Length state
)
{
  switch ( state ) {
    case CPthreadReqSetnameNp_Pre_Length_Fits: {
      /*
       * While the string referenced by the ``name`` parameter fits into the
       * thread name buffer.
       */
      ctx->name = fitting_name;
      break;
    }

    case CPthreadReqSetnameNp_Pre_Length_TooLong: {
      /*
       * While the string referenced by the ``name`` parameter does not fit
       * into the thread name buffer.
       */
      ctx->name = too_long_name;
      break;
    }

    case CPthreadReqSetnameNp_Pre_Length_NA:
      break;
  }
}

static void CPthreadReqSetnameNp_Post_Status_Check(
  CPthreadReqSetnameNp_Context    *ctx,
  CPthreadReqSetnameNp_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetnameNp_Post_Status_Zero: {
      /*
       * The return value of pthread_setname_np() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSetnameNp_Post_Status_ESRCH: {
      /*
       * The return value of pthread_setname_np() shall be equal to ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqSetnameNp_Post_Status_ERANGE: {
      /*
       * The return value of pthread_setname_np() shall be equal to ERANGE.
       */
      T_eq_int( ctx->status, ERANGE );
      break;
    }

    case CPthreadReqSetnameNp_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetnameNp_Post_Name_Check(
  CPthreadReqSetnameNp_Context  *ctx,
  CPthreadReqSetnameNp_Post_Name state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqSetnameNp_Post_Name_Partial: {
      /*
       * The thread name buffer shall contain the first up the maximum thread
       * name length characters of the string referenced by the ``name``
       * parameter.
       */
      eno = pthread_getname_np(
        ctx->thread,
        ctx->name_buf,
        sizeof( ctx->name_buf )
      );
      T_eno_success( eno );

      T_eq_str( ctx->name_buf, "0123456789abcde" );
      break;
    }

    case CPthreadReqSetnameNp_Post_Name_Full: {
      /*
       * The thread name buffer shall contain the string referenced by the
       * ``name`` parameter.
       */
      eno = pthread_getname_np(
        ctx->thread,
        ctx->name_buf,
        sizeof( ctx->name_buf )
      );
      T_eno_success( eno );

      T_eq_str( ctx->name_buf, "foobar" );
      break;
    }

    case CPthreadReqSetnameNp_Post_Name_NA:
      break;
  }
}

static void CPthreadReqSetnameNp_Prepare( CPthreadReqSetnameNp_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
}

static void CPthreadReqSetnameNp_Action( CPthreadReqSetnameNp_Context *ctx )
{
  ctx->status = pthread_setname_np( ctx->thread, ctx->name );
}

static void CPthreadReqSetnameNp_Cleanup( CPthreadReqSetnameNp_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

static const CPthreadReqSetnameNp_Entry
CPthreadReqSetnameNp_Entries[] = {
  { 0, 0, 1, CPthreadReqSetnameNp_Post_Status_ESRCH,
    CPthreadReqSetnameNp_Post_Name_NA },
  { 0, 0, 0, CPthreadReqSetnameNp_Post_Status_Zero,
    CPthreadReqSetnameNp_Post_Name_Full },
  { 0, 0, 0, CPthreadReqSetnameNp_Post_Status_ERANGE,
    CPthreadReqSetnameNp_Post_Name_Partial }
};

static const uint8_t
CPthreadReqSetnameNp_Map[] = {
  0, 0, 1, 2
};

static size_t CPthreadReqSetnameNp_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetnameNp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSetnameNp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSetnameNp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSetnameNp_Scope,
  .initial_context = &CPthreadReqSetnameNp_Instance
};

static inline CPthreadReqSetnameNp_Entry CPthreadReqSetnameNp_PopEntry(
  CPthreadReqSetnameNp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetnameNp_Entries[
    CPthreadReqSetnameNp_Map[ index ]
  ];
}

static void CPthreadReqSetnameNp_SetPreConditionStates(
  CPthreadReqSetnameNp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Length_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqSetnameNp_Pre_Length_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CPthreadReqSetnameNp_TestVariant(
  CPthreadReqSetnameNp_Context *ctx
)
{
  CPthreadReqSetnameNp_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetnameNp_Pre_Length_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetnameNp_Action( ctx );
  CPthreadReqSetnameNp_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSetnameNp_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
}

/**
 * @fn void T_case_body_CPthreadReqSetnameNp( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSetnameNp, &CPthreadReqSetnameNp_Fixture )
{
  CPthreadReqSetnameNp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqSetnameNp_Pre_Thread_NoObj;
    ctx->Map.pci[ 0 ] < CPthreadReqSetnameNp_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqSetnameNp_Pre_Length_Fits;
      ctx->Map.pci[ 1 ] < CPthreadReqSetnameNp_Pre_Length_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqSetnameNp_PopEntry( ctx );
      CPthreadReqSetnameNp_SetPreConditionStates( ctx );
      CPthreadReqSetnameNp_Prepare( ctx );
      CPthreadReqSetnameNp_TestVariant( ctx );
      CPthreadReqSetnameNp_Cleanup( ctx );
    }
  }
}

/** @} */
