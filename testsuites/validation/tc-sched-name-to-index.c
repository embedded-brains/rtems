/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqSchedNameToIndex
 */

/*
 * Copyright (C) 2026 OHB System AG
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

#include <stdint.h>
#include <string.h>
#include <sys/lock.h>

#include <rtems/test.h>

/**
 * @defgroup NewlibReqSchedNameToIndex spec:/newlib/req/sched-name-to-index
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  NewlibReqSchedNameToIndex_Pre_Name_First,
  NewlibReqSchedNameToIndex_Pre_Name_Second,
  NewlibReqSchedNameToIndex_Pre_Name_Third,
  NewlibReqSchedNameToIndex_Pre_Name_Fourth,
  NewlibReqSchedNameToIndex_Pre_Name_Other,
  NewlibReqSchedNameToIndex_Pre_Name_NA
} NewlibReqSchedNameToIndex_Pre_Name;

typedef enum {
  NewlibReqSchedNameToIndex_Pre_Len_Zero,
  NewlibReqSchedNameToIndex_Pre_Len_One,
  NewlibReqSchedNameToIndex_Pre_Len_Short,
  NewlibReqSchedNameToIndex_Pre_Len_Four,
  NewlibReqSchedNameToIndex_Pre_Len_Long,
  NewlibReqSchedNameToIndex_Pre_Len_Max,
  NewlibReqSchedNameToIndex_Pre_Len_NA
} NewlibReqSchedNameToIndex_Pre_Len;

typedef enum {
  NewlibReqSchedNameToIndex_Post_Result_Index,
  NewlibReqSchedNameToIndex_Post_Result_None,
  NewlibReqSchedNameToIndex_Post_Result_NA
} NewlibReqSchedNameToIndex_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Pre_Len_NA : 1;
  uint8_t Post_Result : 2;
} NewlibReqSchedNameToIndex_Entry;

/**
 * @brief Test context for spec:/newlib/req/sched-name-to-index test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the _Sched_Name_to_index()
   *   call.
   */
  int result;

  /**
   * @brief This member contains the index which the call shall return for a
   *   match.
   */
  int expected_index;

  /**
   * @brief This member provides the characters of the string.
   */
  char text[ 8 ];

  /**
   * @brief This member specifies the `name` parameter value.
   */
  const char *name;

  /**
   * @brief This member specifies the `len` parameter value.
   */
  size_t len;

  struct {
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
    NewlibReqSchedNameToIndex_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqSchedNameToIndex_Context;

static NewlibReqSchedNameToIndex_Context NewlibReqSchedNameToIndex_Instance;

static const char *const NewlibReqSchedNameToIndex_PreDesc_Name[] =
  { "First", "Second", "Third", "Fourth", "Other", "NA" };

static const char *const NewlibReqSchedNameToIndex_PreDesc_Len[] =
  { "Zero", "One", "Short", "Four", "Long", "Max", "NA" };

static const char *const *const NewlibReqSchedNameToIndex_PreDesc[] = {
  NewlibReqSchedNameToIndex_PreDesc_Name,
  NewlibReqSchedNameToIndex_PreDesc_Len,
  NULL
};

static void NewlibReqSchedNameToIndex_Pre_Name_Prepare(
  NewlibReqSchedNameToIndex_Context *ctx,
  NewlibReqSchedNameToIndex_Pre_Name state
)
{
  switch ( state ) {
    case NewlibReqSchedNameToIndex_Pre_Name_First: {
      /*
       * While the first four characters of the string referenced by the `name`
       * parameter are the name of the first scheduler of the table of
       * configured schedulers.
       */
      memcpy( ctx->text, "A   ", 4 );
      ctx->expected_index = 0;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Name_Second: {
      /*
       * While the first four characters of the string referenced by the `name`
       * parameter are the name of the second scheduler of the table of
       * configured schedulers.
       */
      memcpy( ctx->text, "B   ", 4 );
      ctx->expected_index = 1;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Name_Third: {
      /*
       * While the first four characters of the string referenced by the `name`
       * parameter are the name of the third scheduler of the table of
       * configured schedulers.
       */
      memcpy( ctx->text, "C   ", 4 );
      ctx->expected_index = 2;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Name_Fourth: {
      /*
       * While the first four characters of the string referenced by the `name`
       * parameter are the name of the fourth scheduler of the table of
       * configured schedulers.
       */
      memcpy( ctx->text, "D   ", 4 );
      ctx->expected_index = 3;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Name_Other: {
      /*
       * While the first four characters of the string referenced by the `name`
       * parameter are not the name of a scheduler of the table of configured
       * schedulers.
       */
      memcpy( ctx->text, "Z   ", 4 );
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Name_NA:
      break;
  }
}

static void NewlibReqSchedNameToIndex_Pre_Len_Prepare(
  NewlibReqSchedNameToIndex_Context *ctx,
  NewlibReqSchedNameToIndex_Pre_Len  state
)
{
  switch ( state ) {
    case NewlibReqSchedNameToIndex_Pre_Len_Zero: {
      /*
       * While the `len` parameter is equal to zero, so that the call builds
       * the name from no character and sets all bytes of the name to zero.
       */
      ctx->len = 0;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Len_One: {
      /*
       * While the `len` parameter is equal to one, so that the call builds the
       * name from one character and sets the remaining bytes of the name to
       * zero.
       */
      ctx->len = 1;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Len_Short: {
      /*
       * While the `len` parameter is greater than one and less than four, so
       * that the call builds the name from the given number of characters and
       * sets the remaining bytes of the name to zero.
       */
      ctx->len = 3;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Len_Four: {
      /*
       * While the `len` parameter is equal to four.
       */
      ctx->len = 4;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Len_Long: {
      /*
       * While the `len` parameter is greater than four and less than SIZE_MAX,
       * so that the call builds the name from the first four characters only.
       */
      ctx->len = 5;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Len_Max: {
      /*
       * While the `len` parameter is equal to SIZE_MAX, so that the call
       * builds the name from the first four characters only.
       */
      ctx->len = SIZE_MAX;
      break;
    }

    case NewlibReqSchedNameToIndex_Pre_Len_NA:
      break;
  }
}

static void NewlibReqSchedNameToIndex_Post_Result_Check(
  NewlibReqSchedNameToIndex_Context    *ctx,
  NewlibReqSchedNameToIndex_Post_Result state
)
{
  switch ( state ) {
    case NewlibReqSchedNameToIndex_Post_Result_Index: {
      /*
       * The return value of _Sched_Name_to_index() shall be the index of the
       * scheduler of the table of configured schedulers whose name is equal to
       * the name which the call builds from the characters of the string
       * referenced by the `name` parameter.
       */
      T_eq_int( ctx->result, ctx->expected_index );
      break;
    }

    case NewlibReqSchedNameToIndex_Post_Result_None: {
      /*
       * The return value of _Sched_Name_to_index() shall be -1.
       */
      T_eq_int( ctx->result, -1 );
      break;
    }

    case NewlibReqSchedNameToIndex_Post_Result_NA:
      break;
  }
}

static void NewlibReqSchedNameToIndex_Prepare(
  NewlibReqSchedNameToIndex_Context *ctx
)
{
  memset( ctx->text, 0, sizeof( ctx->text ) );
  ctx->text[ 4 ] = 'X';
  ctx->result = -2;
  ctx->expected_index = -3;
  ctx->name = ctx->text;
}

static void NewlibReqSchedNameToIndex_Action(
  NewlibReqSchedNameToIndex_Context *ctx
)
{
  ctx->result = _Sched_Name_to_index( ctx->name, ctx->len );
}

/* clang-format off */

static const NewlibReqSchedNameToIndex_Entry
NewlibReqSchedNameToIndex_Entries[] = {
  { 0, 0, 0, NewlibReqSchedNameToIndex_Post_Result_None },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, NewlibReqSchedNameToIndex_Post_Result_NA },
#else
  { 0, 0, 0, NewlibReqSchedNameToIndex_Post_Result_None },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, NewlibReqSchedNameToIndex_Post_Result_NA },
#else
  { 0, 0, 0, NewlibReqSchedNameToIndex_Post_Result_Index },
#endif
  { 0, 0, 0, NewlibReqSchedNameToIndex_Post_Result_Index }
};

static const uint8_t
NewlibReqSchedNameToIndex_Map[] = {
  0, 0, 0, 3, 3, 3, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 0, 0,
  0, 0, 0, 0
};

/* clang-format on */

static size_t NewlibReqSchedNameToIndex_Scope( void *arg, char *buf, size_t n )
{
  NewlibReqSchedNameToIndex_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      NewlibReqSchedNameToIndex_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture NewlibReqSchedNameToIndex_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NewlibReqSchedNameToIndex_Scope,
  .initial_context = &NewlibReqSchedNameToIndex_Instance
};

static inline NewlibReqSchedNameToIndex_Entry
NewlibReqSchedNameToIndex_PopEntry( NewlibReqSchedNameToIndex_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return NewlibReqSchedNameToIndex_Entries
    [ NewlibReqSchedNameToIndex_Map[ index ] ];
}

static void NewlibReqSchedNameToIndex_TestVariant(
  NewlibReqSchedNameToIndex_Context *ctx
)
{
  NewlibReqSchedNameToIndex_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  NewlibReqSchedNameToIndex_Pre_Len_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  NewlibReqSchedNameToIndex_Action( ctx );
  NewlibReqSchedNameToIndex_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_NewlibReqSchedNameToIndex( void )
 */
T_TEST_CASE_FIXTURE(
  NewlibReqSchedNameToIndex,
  &NewlibReqSchedNameToIndex_Fixture
)
{
  NewlibReqSchedNameToIndex_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = NewlibReqSchedNameToIndex_Pre_Name_First;
    ctx->Map.pcs[ 0 ] < NewlibReqSchedNameToIndex_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = NewlibReqSchedNameToIndex_Pre_Len_Zero;
      ctx->Map.pcs[ 1 ] < NewlibReqSchedNameToIndex_Pre_Len_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = NewlibReqSchedNameToIndex_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      NewlibReqSchedNameToIndex_Prepare( ctx );
      NewlibReqSchedNameToIndex_TestVariant( ctx );
    }
  }
}

/** @} */
