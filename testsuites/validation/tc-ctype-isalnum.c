/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsalnum
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
 * @defgroup CCtypeReqIsalnum spec:/c/ctype/req/isalnum
 *
 * @{
 */

typedef enum {
  CCtypeReqIsalnum_Pre_Char_Alnum,
  CCtypeReqIsalnum_Pre_Char_NotAlnum,
  CCtypeReqIsalnum_Pre_Char_NA
} CCtypeReqIsalnum_Pre_Char;

typedef enum {
  CCtypeReqIsalnum_Post_Result_NonZero,
  CCtypeReqIsalnum_Post_Result_Zero,
  CCtypeReqIsalnum_Post_Result_NA
} CCtypeReqIsalnum_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsalnum_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isalnum test case.
 */
typedef struct {
  /**
   * @brief This member contains the characters to process.
   */
  const unsigned char *chars;

  /**
   * @brief This member contains the number of characters to process.
   */
  size_t len;

  /**
   * @brief This member contains the return values.
   */
  int results[ 256 ];

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
    CCtypeReqIsalnum_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsalnum_Context;

static CCtypeReqIsalnum_Context
  CCtypeReqIsalnum_Instance;

static const char * const CCtypeReqIsalnum_PreDesc_Char[] = {
  "Alnum",
  "NotAlnum",
  "NA"
};

static const char * const * const CCtypeReqIsalnum_PreDesc[] = {
  CCtypeReqIsalnum_PreDesc_Char,
  NULL
};

static const unsigned char valid_alnum[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
  'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
  'y', 'z'
};

static const unsigned char valid_not_alnum[] = {
  '\0', 1, 2, 3, 4, 5, 6, '\a', '\b', '\t', '\n', '\v',
  '\f', '\r', 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, ' ', '!', 34, '#',
  '$', '%', '&', 39, '(', ')', '*', '+', ',', '-', '.', '/',
  ':', ';', '<', '=', '>', '?', '@', '[', 92, ']', '^', '_',
  '`', '{', '|', '}', '~', 127, 128, 129, 130, 131, 132, 133,
  134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
  146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181,
  182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
  194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
  206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217,
  218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
  230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241,
  242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253,
  254, 255
};

static void CCtypeReqIsalnum_Pre_Char_Prepare(
  CCtypeReqIsalnum_Context *ctx,
  CCtypeReqIsalnum_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIsalnum_Pre_Char_Alnum: {
      /*
       * While the character specified by `c` is an alphanumeric character.
       */
      ctx->chars = valid_alnum;
      ctx->len = sizeof( valid_alnum );
      break;
    }

    case CCtypeReqIsalnum_Pre_Char_NotAlnum: {
      /*
       * While the character specified by `c` is not an alphanumeric character.
       */
      ctx->chars = valid_not_alnum;
      ctx->len = sizeof( valid_not_alnum );
      break;
    }

    case CCtypeReqIsalnum_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIsalnum_Post_Result_Check(
  CCtypeReqIsalnum_Context    *ctx,
  CCtypeReqIsalnum_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsalnum_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      size_t i;
      for ( i = 0; i < ctx->len; ++i ) {
        T_ne_int( ctx->results[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsalnum_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      size_t i;
      for ( i = 0; i < ctx->len; ++i ) {
        T_eq_int( ctx->results[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsalnum_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsalnum_Action( CCtypeReqIsalnum_Context *ctx )
{
  size_t i;
  for ( i = 0; i < ctx->len; ++i ) {
    ctx->results[ i ] = isalnum( ctx->chars[ i ] );
  }
}

static const CCtypeReqIsalnum_Entry
CCtypeReqIsalnum_Entries[] = {
  { 0, 0, CCtypeReqIsalnum_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsalnum_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsalnum_Map[] = {
  0, 1
};

static size_t CCtypeReqIsalnum_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsalnum_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsalnum_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsalnum_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsalnum_Scope,
  .initial_context = &CCtypeReqIsalnum_Instance
};

static inline CCtypeReqIsalnum_Entry CCtypeReqIsalnum_PopEntry(
  CCtypeReqIsalnum_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsalnum_Entries[
    CCtypeReqIsalnum_Map[ index ]
  ];
}

static void CCtypeReqIsalnum_TestVariant( CCtypeReqIsalnum_Context *ctx )
{
  CCtypeReqIsalnum_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsalnum_Action( ctx );
  CCtypeReqIsalnum_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsalnum( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsalnum, &CCtypeReqIsalnum_Fixture )
{
  CCtypeReqIsalnum_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsalnum_Pre_Char_Alnum;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsalnum_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsalnum_PopEntry( ctx );
    CCtypeReqIsalnum_TestVariant( ctx );
  }
}

/** @} */
