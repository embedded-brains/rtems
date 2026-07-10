/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIscntrl
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
#include <stdio.h>

#include <rtems/test.h>

/**
 * @defgroup CCtypeReqIscntrl spec:/c/ctype/req/iscntrl
 *
 * @{
 */

typedef enum {
  CCtypeReqIscntrl_Pre_Char_Cntrl,
  CCtypeReqIscntrl_Pre_Char_NotCntrl,
  CCtypeReqIscntrl_Pre_Char_EOF,
  CCtypeReqIscntrl_Pre_Char_NA
} CCtypeReqIscntrl_Pre_Char;

typedef enum {
  CCtypeReqIscntrl_Post_Result_NonZero,
  CCtypeReqIscntrl_Post_Result_Zero,
  CCtypeReqIscntrl_Post_Result_NA
} CCtypeReqIscntrl_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIscntrl_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/iscntrl test case.
 */
typedef struct {
  /**
   * @brief This member contains the characters to test.
   */
  const int *chars;

  /**
   * @brief This member contains the number of characters to test.
   */
  size_t len;

  /**
   * @brief This member contains the result of the function call.
   */
  int result[ 256 ];

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
    CCtypeReqIscntrl_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIscntrl_Context;

static CCtypeReqIscntrl_Context
  CCtypeReqIscntrl_Instance;

static const char * const CCtypeReqIscntrl_PreDesc_Char[] = {
  "Cntrl",
  "NotCntrl",
  "EOF",
  "NA"
};

static const char * const * const CCtypeReqIscntrl_PreDesc[] = {
  CCtypeReqIscntrl_PreDesc_Char,
  NULL
};

static const int cntrl_chars[] = {
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a',
  '\b', '\t', '\n', '\v', '\f', '\r', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
  '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x7f'
};

static const int not_cntrl_chars[] = {
  ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff'
};

static const int eof_char[] = { EOF };

static void CCtypeReqIscntrl_Pre_Char_Prepare(
  CCtypeReqIscntrl_Context *ctx,
  CCtypeReqIscntrl_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIscntrl_Pre_Char_Cntrl: {
      /*
       * While the character specified by c is a control character.
       */
      ctx->chars = cntrl_chars;
      ctx->len = RTEMS_ARRAY_SIZE( cntrl_chars );
      break;
    }

    case CCtypeReqIscntrl_Pre_Char_NotCntrl: {
      /*
       * While the character specified by c is an 8-bit character that is not a
       * control character.
       */
      ctx->chars = not_cntrl_chars;
      ctx->len = RTEMS_ARRAY_SIZE( not_cntrl_chars );
      break;
    }

    case CCtypeReqIscntrl_Pre_Char_EOF: {
      /*
       * While the character specified by c is EOF.
       */
      ctx->chars = eof_char;
      ctx->len = RTEMS_ARRAY_SIZE( eof_char );
      break;
    }

    case CCtypeReqIscntrl_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIscntrl_Post_Result_Check(
  CCtypeReqIscntrl_Context    *ctx,
  CCtypeReqIscntrl_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIscntrl_Post_Result_NonZero: {
      /*
       * The return value shall be a non-zero value.
       */
      for ( size_t i = 0; i < ctx->len; ++i ) {
        T_ne_int( ctx->result[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIscntrl_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      for ( size_t i = 0; i < ctx->len; ++i ) {
        T_eq_int( ctx->result[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIscntrl_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIscntrl_Action( CCtypeReqIscntrl_Context *ctx )
{
  for ( size_t i = 0; i < ctx->len; ++i ) {
    int c;

    c = ctx->chars[ i ];
    ctx->result[ i ] = iscntrl( c );
  }
}

static const CCtypeReqIscntrl_Entry
CCtypeReqIscntrl_Entries[] = {
  { 0, 0, CCtypeReqIscntrl_Post_Result_Zero },
  { 0, 0, CCtypeReqIscntrl_Post_Result_NonZero }
};

static const uint8_t
CCtypeReqIscntrl_Map[] = {
  1, 0, 0
};

static size_t CCtypeReqIscntrl_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIscntrl_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIscntrl_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIscntrl_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIscntrl_Scope,
  .initial_context = &CCtypeReqIscntrl_Instance
};

static inline CCtypeReqIscntrl_Entry CCtypeReqIscntrl_PopEntry(
  CCtypeReqIscntrl_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIscntrl_Entries[
    CCtypeReqIscntrl_Map[ index ]
  ];
}

static void CCtypeReqIscntrl_TestVariant( CCtypeReqIscntrl_Context *ctx )
{
  CCtypeReqIscntrl_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIscntrl_Action( ctx );
  CCtypeReqIscntrl_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIscntrl( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIscntrl, &CCtypeReqIscntrl_Fixture )
{
  CCtypeReqIscntrl_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIscntrl_Pre_Char_Cntrl;
    ctx->Map.pcs[ 0 ] < CCtypeReqIscntrl_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIscntrl_PopEntry( ctx );
    CCtypeReqIscntrl_TestVariant( ctx );
  }
}

/** @} */
