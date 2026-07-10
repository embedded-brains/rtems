/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqToupper
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

#include <ctype.h>

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqToupper spec:/c/ctype/req/toupper
 *
 * @{
 */

typedef enum {
  CCtypeReqToupper_Pre_Character_Lower,
  CCtypeReqToupper_Pre_Character_Other,
  CCtypeReqToupper_Pre_Character_NA
} CCtypeReqToupper_Pre_Character;

typedef enum {
  CCtypeReqToupper_Post_Result_Upper,
  CCtypeReqToupper_Post_Result_Original,
  CCtypeReqToupper_Post_Result_NA
} CCtypeReqToupper_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Character_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqToupper_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/toupper test case.
 */
typedef struct {
  /**
   * @brief This member contains the character array to be tested.
   */
  const int *c_array;

  /**
   * @brief This member contains the size of the character array.
   */
  size_t c_array_size;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    CCtypeReqToupper_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqToupper_Context;

static CCtypeReqToupper_Context
  CCtypeReqToupper_Instance;

static const char * const CCtypeReqToupper_PreDesc_Character[] = {
  "Lower",
  "Other",
  "NA"
};

static const char * const * const CCtypeReqToupper_PreDesc[] = {
  CCtypeReqToupper_PreDesc_Character,
  NULL
};

static const int lower_cases[] = {
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

static const int other_cases[] = {
  -1, '\0', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
  '\b', '\t', '\n', '\v', '\f', '\r', '\x0e', '\x0f', '\x10', '\x11',
  '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19',
  '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', ' ', '!', '"', '#',
  '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0',
  '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
  '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~',
  '\x7f', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86',
  '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e',
  '\x8f', '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96',
  '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e',
  '\x9f', '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6',
  '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae',
  '\xaf', '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6',
  '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe',
  '\xbf', '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6',
  '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce',
  '\xcf', '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6',
  '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde',
  '\xdf', '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6',
  '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee',
  '\xef', '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6',
  '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe',
  '\xff'
};

static void CCtypeReqToupper_Pre_Character_Prepare(
  CCtypeReqToupper_Context      *ctx,
  CCtypeReqToupper_Pre_Character state
)
{
  switch ( state ) {
    case CCtypeReqToupper_Pre_Character_Lower: {
      /*
       * While the character specified by `c` is a lowercase letter.
       */
      ctx->c_array = lower_cases;
      ctx->c_array_size = sizeof( lower_cases ) / sizeof( lower_cases[ 0 ] );
      break;
    }

    case CCtypeReqToupper_Pre_Character_Other: {
      /*
       * While the character specified by `c` is not a lowercase letter.
       */
      ctx->c_array = other_cases;
      ctx->c_array_size = sizeof( other_cases ) / sizeof( other_cases[ 0 ] );
      break;
    }

    case CCtypeReqToupper_Pre_Character_NA:
      break;
  }
}

static void CCtypeReqToupper_Post_Result_Check(
  CCtypeReqToupper_Context    *ctx,
  CCtypeReqToupper_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqToupper_Post_Result_Upper: {
      /*
       * The return value shall be the corresponding uppercase letter of the
       * character specified by `c`.
       */
      for ( size_t i = 0; i < ctx->c_array_size; ++i ) {
        int retval;

        retval = toupper( ctx->c_array[ i ] );
        T_eq_int( retval, ctx->c_array[ i ] - 'a' + 'A' );
      }
      break;
    }

    case CCtypeReqToupper_Post_Result_Original: {
      /*
       * The return value shall be the original character specified by `c`.
       */
      for ( size_t i = 0; i < ctx->c_array_size; ++i ) {
        int retval;

        retval = toupper( ctx->c_array[ i ] );
        T_eq_int( retval, ctx->c_array[ i ] );
      }
      break;
    }

    case CCtypeReqToupper_Post_Result_NA:
      break;
  }
}

static void CCtypeReqToupper_Action( void )
{
  /* The action is performed in the post-condition checks. */
}

static const CCtypeReqToupper_Entry
CCtypeReqToupper_Entries[] = {
  { 0, 0, CCtypeReqToupper_Post_Result_Upper },
  { 0, 0, CCtypeReqToupper_Post_Result_Original }
};

static const uint8_t
CCtypeReqToupper_Map[] = {
  0, 1
};

static size_t CCtypeReqToupper_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqToupper_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqToupper_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqToupper_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqToupper_Scope,
  .initial_context = &CCtypeReqToupper_Instance
};

static inline CCtypeReqToupper_Entry CCtypeReqToupper_PopEntry(
  CCtypeReqToupper_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqToupper_Entries[
    CCtypeReqToupper_Map[ index ]
  ];
}

static void CCtypeReqToupper_TestVariant( CCtypeReqToupper_Context *ctx )
{
  CCtypeReqToupper_Pre_Character_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqToupper_Action();
  CCtypeReqToupper_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqToupper( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqToupper, &CCtypeReqToupper_Fixture )
{
  CCtypeReqToupper_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqToupper_Pre_Character_Lower;
    ctx->Map.pcs[ 0 ] < CCtypeReqToupper_Pre_Character_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqToupper_PopEntry( ctx );
    CCtypeReqToupper_TestVariant( ctx );
  }
}

/** @} */
