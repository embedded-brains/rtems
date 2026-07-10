/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsprint
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
 * @defgroup CCtypeReqIsprint spec:/c/ctype/req/isprint
 *
 * @{
 */

typedef enum {
  CCtypeReqIsprint_Pre_Char_Print,
  CCtypeReqIsprint_Pre_Char_NotPrint,
  CCtypeReqIsprint_Pre_Char_NA
} CCtypeReqIsprint_Pre_Char;

typedef enum {
  CCtypeReqIsprint_Post_Result_NonZero,
  CCtypeReqIsprint_Post_Result_Zero,
  CCtypeReqIsprint_Post_Result_NA
} CCtypeReqIsprint_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsprint_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isprint test case.
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
    CCtypeReqIsprint_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsprint_Context;

static CCtypeReqIsprint_Context
  CCtypeReqIsprint_Instance;

static const char * const CCtypeReqIsprint_PreDesc_Char[] = {
  "Print",
  "NotPrint",
  "NA"
};

static const char * const * const CCtypeReqIsprint_PreDesc[] = {
  CCtypeReqIsprint_PreDesc_Char,
  NULL
};

static const unsigned char valid_print[] = {
  ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~'
};

static const unsigned char valid_not_print[] = {
  '\0', 1, 2, 3, 4, 5, 6, '\a', '\b', '\t', '\n', '\v',
  '\f', '\r', 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, 127, 128, 129, 130, 131, 132, 133,
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

static void CCtypeReqIsprint_Pre_Char_Prepare(
  CCtypeReqIsprint_Context *ctx,
  CCtypeReqIsprint_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIsprint_Pre_Char_Print: {
      /*
       * While the character specified by `c` is a printable character.
       */
      ctx->chars = valid_print;
      ctx->len = sizeof( valid_print );
      break;
    }

    case CCtypeReqIsprint_Pre_Char_NotPrint: {
      /*
       * While the character specified by `c` is not a printable character.
       */
      ctx->chars = valid_not_print;
      ctx->len = sizeof( valid_not_print );
      break;
    }

    case CCtypeReqIsprint_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIsprint_Post_Result_Check(
  CCtypeReqIsprint_Context    *ctx,
  CCtypeReqIsprint_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsprint_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      size_t i;
      for ( i = 0; i < ctx->len; ++i ) {
        T_ne_int( ctx->results[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsprint_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      size_t i;
      for ( i = 0; i < ctx->len; ++i ) {
        T_eq_int( ctx->results[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsprint_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsprint_Action( CCtypeReqIsprint_Context *ctx )
{
  size_t i;
  for ( i = 0; i < ctx->len; ++i ) {
    ctx->results[ i ] = isprint( ctx->chars[ i ] );
  }
}

static const CCtypeReqIsprint_Entry
CCtypeReqIsprint_Entries[] = {
  { 0, 0, CCtypeReqIsprint_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsprint_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsprint_Map[] = {
  0, 1
};

static size_t CCtypeReqIsprint_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsprint_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsprint_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsprint_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsprint_Scope,
  .initial_context = &CCtypeReqIsprint_Instance
};

static inline CCtypeReqIsprint_Entry CCtypeReqIsprint_PopEntry(
  CCtypeReqIsprint_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsprint_Entries[
    CCtypeReqIsprint_Map[ index ]
  ];
}

static void CCtypeReqIsprint_TestVariant( CCtypeReqIsprint_Context *ctx )
{
  CCtypeReqIsprint_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsprint_Action( ctx );
  CCtypeReqIsprint_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsprint( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsprint, &CCtypeReqIsprint_Fixture )
{
  CCtypeReqIsprint_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsprint_Pre_Char_Print;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsprint_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsprint_PopEntry( ctx );
    CCtypeReqIsprint_TestVariant( ctx );
  }
}

/** @} */
