/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqGetnameNp
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
 * @defgroup CPthreadReqGetnameNp spec:/c/pthread/req/getname-np
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqGetnameNp_Pre_Thread_NoObj,
  CPthreadReqGetnameNp_Pre_Thread_Valid,
  CPthreadReqGetnameNp_Pre_Thread_NA
} CPthreadReqGetnameNp_Pre_Thread;

typedef enum {
  CPthreadReqGetnameNp_Pre_ThreadName_Empty,
  CPthreadReqGetnameNp_Pre_ThreadName_NonEmpty,
  CPthreadReqGetnameNp_Pre_ThreadName_NA
} CPthreadReqGetnameNp_Pre_ThreadName;

typedef enum {
  CPthreadReqGetnameNp_Pre_NameLen_NullZero,
  CPthreadReqGetnameNp_Pre_NameLen_ValidZero,
  CPthreadReqGetnameNp_Pre_NameLen_Valid,
  CPthreadReqGetnameNp_Pre_NameLen_NA
} CPthreadReqGetnameNp_Pre_NameLen;

typedef enum {
  CPthreadReqGetnameNp_Pre_ThreadNameLength_Fits,
  CPthreadReqGetnameNp_Pre_ThreadNameLength_TooLong,
  CPthreadReqGetnameNp_Pre_ThreadNameLength_NA
} CPthreadReqGetnameNp_Pre_ThreadNameLength;

typedef enum {
  CPthreadReqGetnameNp_Post_Status_Zero,
  CPthreadReqGetnameNp_Post_Status_ESRCH,
  CPthreadReqGetnameNp_Post_Status_ERANGE,
  CPthreadReqGetnameNp_Post_Status_NA
} CPthreadReqGetnameNp_Post_Status;

typedef enum {
  CPthreadReqGetnameNp_Post_Name_Empty,
  CPthreadReqGetnameNp_Post_Name_PartialObject,
  CPthreadReqGetnameNp_Post_Name_FullObject,
  CPthreadReqGetnameNp_Post_Name_PartialString,
  CPthreadReqGetnameNp_Post_Name_FullString,
  CPthreadReqGetnameNp_Post_Name_NA
} CPthreadReqGetnameNp_Post_Name;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Thread_NA : 1;
  uint16_t Pre_ThreadName_NA : 1;
  uint16_t Pre_NameLen_NA : 1;
  uint16_t Pre_ThreadNameLength_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Name : 3;
} CPthreadReqGetnameNp_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/getname-np test case.
 */
typedef struct {
  /**
   * @brief This member provides a task with a name.
   */
  rtems_id worker_id;

  /**
   * @brief This member provides a buffer for the thread name.
   */
  char name_buf[ 7 ];;

  /**
   * @brief This member contains the expected name.
   */
  char expected_name[ 5 ];;

  /**
   * @brief This member provides the ``thread`` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member provides the ``name`` parameter value.
   */
  char *name;

  /**
   * @brief This member provides the ``len`` parameter value.
   */
  size_t len;

  /**
   * @brief This member contains the returned status.
   */
  int status;

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
    CPthreadReqGetnameNp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqGetnameNp_Context;

static CPthreadReqGetnameNp_Context
  CPthreadReqGetnameNp_Instance;

static const char * const CPthreadReqGetnameNp_PreDesc_Thread[] = {
  "NoObj",
  "Valid",
  "NA"
};

static const char * const CPthreadReqGetnameNp_PreDesc_ThreadName[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const CPthreadReqGetnameNp_PreDesc_NameLen[] = {
  "NullZero",
  "ValidZero",
  "Valid",
  "NA"
};

static const char * const CPthreadReqGetnameNp_PreDesc_ThreadNameLength[] = {
  "Fits",
  "TooLong",
  "NA"
};

static const char * const * const CPthreadReqGetnameNp_PreDesc[] = {
  CPthreadReqGetnameNp_PreDesc_Thread,
  CPthreadReqGetnameNp_PreDesc_ThreadName,
  CPthreadReqGetnameNp_PreDesc_NameLen,
  CPthreadReqGetnameNp_PreDesc_ThreadNameLength,
  NULL
};

static void CPthreadReqGetnameNp_Pre_Thread_Prepare(
  CPthreadReqGetnameNp_Context   *ctx,
  CPthreadReqGetnameNp_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqGetnameNp_Pre_Thread_NoObj: {
      /*
       * While the ``thread`` parameter is not associated with a thread.
       */
      ctx->thread = 0xffffffff;
      break;
    }

    case CPthreadReqGetnameNp_Pre_Thread_Valid: {
      /*
       * While the ``thread`` parameter is associated with a thread.
       */
      ctx->thread = ctx->worker_id;
      break;
    }

    case CPthreadReqGetnameNp_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqGetnameNp_Pre_ThreadName_Prepare(
  CPthreadReqGetnameNp_Context       *ctx,
  CPthreadReqGetnameNp_Pre_ThreadName state
)
{
  int eno;

  switch ( state ) {
    case CPthreadReqGetnameNp_Pre_ThreadName_Empty: {
      /*
       * While the thread has the empty string name.
       */
      eno = pthread_setname_np( ctx->worker_id, "" );
      T_eno_success( eno );
      break;
    }

    case CPthreadReqGetnameNp_Pre_ThreadName_NonEmpty: {
      /*
       * While the thread has a non-empty string name.
       */
      eno = pthread_setname_np( ctx->worker_id, "string" );
      T_eno_success( eno );
      break;
    }

    case CPthreadReqGetnameNp_Pre_ThreadName_NA:
      break;
  }
}

static void CPthreadReqGetnameNp_Pre_NameLen_Prepare(
  CPthreadReqGetnameNp_Context    *ctx,
  CPthreadReqGetnameNp_Pre_NameLen state
)
{
  switch ( state ) {
    case CPthreadReqGetnameNp_Pre_NameLen_NullZero: {
      /*
       * While the ``name`` parameter is NULL, while the ``len`` parameter is
       * zero.
       */
      ctx->name = NULL;
      ctx->len = 0;
      break;
    }

    case CPthreadReqGetnameNp_Pre_NameLen_ValidZero: {
      /*
       * While the ``name`` parameter references a buffer, while the ``len``
       * parameter is zero.
       */
      ctx->name = ctx->name_buf;
      ctx->len = 0;
      break;
    }

    case CPthreadReqGetnameNp_Pre_NameLen_Valid: {
      /*
       * While the ``name`` parameter references a buffer, while the ``len``
       * parameter is greater than zero, while the ``len`` parameter is equal
       * to the size in bytes of the buffer referenced by the ``name``
       * parameter.
       */
      ctx->name = ctx->name_buf;
      ctx->len = RTEMS_ARRAY_SIZE( ctx->name_buf );
      break;
    }

    case CPthreadReqGetnameNp_Pre_NameLen_NA:
      break;
  }
}

static void CPthreadReqGetnameNp_Pre_ThreadNameLength_Prepare(
  CPthreadReqGetnameNp_Context             *ctx,
  CPthreadReqGetnameNp_Pre_ThreadNameLength state
)
{
  switch ( state ) {
    case CPthreadReqGetnameNp_Pre_ThreadNameLength_Fits: {
      /*
       * While the thread name fits into the buffer referenced by the ``name``
       * parameter.
       */
      /* Already prepared */
      break;
    }

    case CPthreadReqGetnameNp_Pre_ThreadNameLength_TooLong: {
      /*
       * While the thread name does not fit into the buffer referenced by the
       * ``name`` parameter.
       */
      if ( ctx->len > 0 ) {
        ctx->len = 2;
      }
      break;
    }

    case CPthreadReqGetnameNp_Pre_ThreadNameLength_NA:
      break;
  }
}

static void CPthreadReqGetnameNp_Post_Status_Check(
  CPthreadReqGetnameNp_Context    *ctx,
  CPthreadReqGetnameNp_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqGetnameNp_Post_Status_Zero: {
      /*
       * The return value of pthread_getname_np() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqGetnameNp_Post_Status_ESRCH: {
      /*
       * The return value of pthread_getname_np() shall be equal to ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqGetnameNp_Post_Status_ERANGE: {
      /*
       * The return value of pthread_getname_np() shall be equal to ERANGE.
       */
      T_eq_int( ctx->status, ERANGE );
      break;
    }

    case CPthreadReqGetnameNp_Post_Status_NA:
      break;
  }
}

static void CPthreadReqGetnameNp_Post_Name_Check(
  CPthreadReqGetnameNp_Context  *ctx,
  CPthreadReqGetnameNp_Post_Name state
)
{
  switch ( state ) {
    case CPthreadReqGetnameNp_Post_Name_Empty: {
      /*
       * The string referenced by the ``name`` parameter shall contain the
       * empty string.
       */
      T_eq_str( ctx->name_buf, "" );
      break;
    }

    case CPthreadReqGetnameNp_Post_Name_PartialObject: {
      /*
       * The string referenced by the ``name`` parameter shall contain the
       * first up to ``len`` parameter value minus one characters of the thread
       * object name.
       */
      T_eq_str( ctx->name_buf, "W" );
      break;
    }

    case CPthreadReqGetnameNp_Post_Name_FullObject: {
      /*
       * The string referenced by the ``name`` parameter shall contain the
       * thread object name.
       */
      T_eq_str( ctx->name_buf, "WORK" );
      break;
    }

    case CPthreadReqGetnameNp_Post_Name_PartialString: {
      /*
       * The string referenced by the ``name`` parameter shall contain the
       * first up to ``len`` parameter value minus one characters of the thread
       * string name.
       */
      T_eq_str( ctx->name_buf, "s" );
      break;
    }

    case CPthreadReqGetnameNp_Post_Name_FullString: {
      /*
       * The string referenced by the ``name`` parameter shall contain the
       * thread string name.
       */
      T_eq_str( ctx->name_buf, "string" );
      break;
    }

    case CPthreadReqGetnameNp_Post_Name_NA:
      break;
  }
}

static void CPthreadReqGetnameNp_Setup( CPthreadReqGetnameNp_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
}

static void CPthreadReqGetnameNp_Setup_Wrap( void *arg )
{
  CPthreadReqGetnameNp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetnameNp_Setup( ctx );
}

static void CPthreadReqGetnameNp_Teardown( CPthreadReqGetnameNp_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

static void CPthreadReqGetnameNp_Teardown_Wrap( void *arg )
{
  CPthreadReqGetnameNp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqGetnameNp_Teardown( ctx );
}

static void CPthreadReqGetnameNp_Prepare( CPthreadReqGetnameNp_Context *ctx )
{
  memset( ctx->name_buf, 0xff, sizeof( ctx->name_buf ) );
}

static void CPthreadReqGetnameNp_Action( CPthreadReqGetnameNp_Context *ctx )
{
  ctx->status = pthread_getname_np( ctx->thread, ctx->name, ctx->len );
}

static const CPthreadReqGetnameNp_Entry
CPthreadReqGetnameNp_Entries[] = {
  { 0, 0, 1, 0, 1, CPthreadReqGetnameNp_Post_Status_ESRCH,
    CPthreadReqGetnameNp_Post_Name_NA },
  { 0, 0, 1, 0, 1, CPthreadReqGetnameNp_Post_Status_ESRCH,
    CPthreadReqGetnameNp_Post_Name_Empty },
  { 0, 0, 0, 0, 1, CPthreadReqGetnameNp_Post_Status_ERANGE,
    CPthreadReqGetnameNp_Post_Name_NA },
  { 0, 0, 0, 0, 0, CPthreadReqGetnameNp_Post_Status_ERANGE,
    CPthreadReqGetnameNp_Post_Name_NA },
  { 0, 0, 0, 0, 0, CPthreadReqGetnameNp_Post_Status_Zero,
    CPthreadReqGetnameNp_Post_Name_FullObject },
  { 0, 0, 0, 0, 0, CPthreadReqGetnameNp_Post_Status_ERANGE,
    CPthreadReqGetnameNp_Post_Name_PartialObject },
  { 0, 0, 0, 0, 0, CPthreadReqGetnameNp_Post_Status_Zero,
    CPthreadReqGetnameNp_Post_Name_FullString },
  { 0, 0, 0, 0, 0, CPthreadReqGetnameNp_Post_Status_ERANGE,
    CPthreadReqGetnameNp_Post_Name_PartialString }
};

static const uint8_t
CPthreadReqGetnameNp_Map[] = {
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 4, 5, 2, 3, 2, 3, 6, 7
};

static size_t CPthreadReqGetnameNp_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqGetnameNp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqGetnameNp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqGetnameNp_Fixture = {
  .setup = CPthreadReqGetnameNp_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqGetnameNp_Teardown_Wrap,
  .scope = CPthreadReqGetnameNp_Scope,
  .initial_context = &CPthreadReqGetnameNp_Instance
};

static inline CPthreadReqGetnameNp_Entry CPthreadReqGetnameNp_PopEntry(
  CPthreadReqGetnameNp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqGetnameNp_Entries[
    CPthreadReqGetnameNp_Map[ index ]
  ];
}

static void CPthreadReqGetnameNp_SetPreConditionStates(
  CPthreadReqGetnameNp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_ThreadName_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqGetnameNp_Pre_ThreadName_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_ThreadNameLength_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqGetnameNp_Pre_ThreadNameLength_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CPthreadReqGetnameNp_TestVariant(
  CPthreadReqGetnameNp_Context *ctx
)
{
  CPthreadReqGetnameNp_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqGetnameNp_Pre_ThreadName_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqGetnameNp_Pre_NameLen_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqGetnameNp_Pre_ThreadNameLength_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqGetnameNp_Action( ctx );
  CPthreadReqGetnameNp_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqGetnameNp_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
}

/**
 * @fn void T_case_body_CPthreadReqGetnameNp( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqGetnameNp, &CPthreadReqGetnameNp_Fixture )
{
  CPthreadReqGetnameNp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqGetnameNp_Pre_Thread_NoObj;
    ctx->Map.pci[ 0 ] < CPthreadReqGetnameNp_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqGetnameNp_Pre_ThreadName_Empty;
      ctx->Map.pci[ 1 ] < CPthreadReqGetnameNp_Pre_ThreadName_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqGetnameNp_Pre_NameLen_NullZero;
        ctx->Map.pci[ 2 ] < CPthreadReqGetnameNp_Pre_NameLen_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqGetnameNp_Pre_ThreadNameLength_Fits;
          ctx->Map.pci[ 3 ] < CPthreadReqGetnameNp_Pre_ThreadNameLength_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CPthreadReqGetnameNp_PopEntry( ctx );
          CPthreadReqGetnameNp_SetPreConditionStates( ctx );
          CPthreadReqGetnameNp_Prepare( ctx );
          CPthreadReqGetnameNp_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
