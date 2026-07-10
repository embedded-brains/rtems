/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsalpha
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
 * @defgroup CCtypeReqIsalpha spec:/c/ctype/req/isalpha
 *
 * @{
 */

typedef enum {
  CCtypeReqIsalpha_Pre_Char_Upper,
  CCtypeReqIsalpha_Pre_Char_Lower,
  CCtypeReqIsalpha_Pre_Char_Other,
  CCtypeReqIsalpha_Pre_Char_NA
} CCtypeReqIsalpha_Pre_Char;

typedef enum {
  CCtypeReqIsalpha_Post_Result_NonZero,
  CCtypeReqIsalpha_Post_Result_Zero,
  CCtypeReqIsalpha_Post_Result_NA
} CCtypeReqIsalpha_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsalpha_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isalpha test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `c` parameter value.
   */
  int c;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member points to the character set to test.
   */
  const int *char_set;

  /**
   * @brief This member contains the number of characters in the set to test.
   */
  int char_count;

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
    CCtypeReqIsalpha_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsalpha_Context;

static CCtypeReqIsalpha_Context
  CCtypeReqIsalpha_Instance;

static const char * const CCtypeReqIsalpha_PreDesc_Char[] = {
  "Upper",
  "Lower",
  "Other",
  "NA"
};

static const char * const * const CCtypeReqIsalpha_PreDesc[] = {
  CCtypeReqIsalpha_PreDesc_Char,
  NULL
};

static const int upper_chars[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

static const int lower_chars[] = {
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

static const int other_chars[] = {
  -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, ' ', '!', '"', '#', '$',
  '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', '[', '\\',
  ']', '^', '_', '`', '{', '|', '}', '~', 127, 128, 129, 130, 131, 132, 133,
  134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
  149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163,
  164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178,
  179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
  194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
  209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
  224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238,
  239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253,
  254, 255
};

static void CCtypeReqIsalpha_Pre_Char_Prepare(
  CCtypeReqIsalpha_Context *ctx,
  CCtypeReqIsalpha_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIsalpha_Pre_Char_Upper: {
      /*
       * While the character specified by `c` is an uppercase letter.
       */
      ctx->char_set = upper_chars;
      ctx->char_count = sizeof( upper_chars ) / sizeof( upper_chars[ 0 ] );
      break;
    }

    case CCtypeReqIsalpha_Pre_Char_Lower: {
      /*
       * While the character specified by `c` is a lowercase letter.
       */
      ctx->char_set = lower_chars;
      ctx->char_count = sizeof( lower_chars ) / sizeof( lower_chars[ 0 ] );
      break;
    }

    case CCtypeReqIsalpha_Pre_Char_Other: {
      /*
       * While the character specified by `c` is not an uppercase letter, while
       * the character specified by `c` is not a lowercase letter.
       */
      ctx->char_set = other_chars;
      ctx->char_count = sizeof( other_chars ) / sizeof( other_chars[ 0 ] );
      break;
    }

    case CCtypeReqIsalpha_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIsalpha_Post_Result_Check(
  CCtypeReqIsalpha_Context    *ctx,
  CCtypeReqIsalpha_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsalpha_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      int i;

      for ( i = 0; i < ctx->char_count; ++i ) {
        ctx->c = ctx->char_set[ i ];
        ctx->retval = isalpha( ctx->c );
        T_ne_int( ctx->retval, 0 );
      }
      break;
    }

    case CCtypeReqIsalpha_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      int i;

      for ( i = 0; i < ctx->char_count; ++i ) {
        ctx->c = ctx->char_set[ i ];
        ctx->retval = isalpha( ctx->c );
        T_eq_int( ctx->retval, 0 );
      }
      break;
    }

    case CCtypeReqIsalpha_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsalpha_Action( void )
{
  /* The action is performed in the post-condition checks to test all characters in the set. */
}

static const CCtypeReqIsalpha_Entry
CCtypeReqIsalpha_Entries[] = {
  { 0, 0, CCtypeReqIsalpha_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsalpha_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsalpha_Map[] = {
  0, 0, 1
};

static size_t CCtypeReqIsalpha_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsalpha_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsalpha_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsalpha_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsalpha_Scope,
  .initial_context = &CCtypeReqIsalpha_Instance
};

static inline CCtypeReqIsalpha_Entry CCtypeReqIsalpha_PopEntry(
  CCtypeReqIsalpha_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsalpha_Entries[
    CCtypeReqIsalpha_Map[ index ]
  ];
}

static void CCtypeReqIsalpha_TestVariant( CCtypeReqIsalpha_Context *ctx )
{
  CCtypeReqIsalpha_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsalpha_Action();
  CCtypeReqIsalpha_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsalpha( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsalpha, &CCtypeReqIsalpha_Fixture )
{
  CCtypeReqIsalpha_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsalpha_Pre_Char_Upper;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsalpha_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsalpha_PopEntry( ctx );
    CCtypeReqIsalpha_TestVariant( ctx );
  }
}

/** @} */
