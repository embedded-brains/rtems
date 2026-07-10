/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsxdigit
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
 * @defgroup CCtypeReqIsxdigit spec:/c/ctype/req/isxdigit
 *
 * @{
 */

typedef enum {
  CCtypeReqIsxdigit_Pre_Char_HexLetter,
  CCtypeReqIsxdigit_Pre_Char_Digit,
  CCtypeReqIsxdigit_Pre_Char_Other,
  CCtypeReqIsxdigit_Pre_Char_NA
} CCtypeReqIsxdigit_Pre_Char;

typedef enum {
  CCtypeReqIsxdigit_Post_Result_NonZero,
  CCtypeReqIsxdigit_Post_Result_Zero,
  CCtypeReqIsxdigit_Post_Result_NA
} CCtypeReqIsxdigit_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsxdigit_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isxdigit test case.
 */
typedef struct {
  /**
   * @brief This member contains the characters to be tested.
   */
  const int *chars;

  /**
   * @brief This member contains the number of characters to be tested.
   */
  size_t chars_len;

  /**
   * @brief This member contains the return values of the function.
   */
  int retval[ 257 ];

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
    CCtypeReqIsxdigit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsxdigit_Context;

static CCtypeReqIsxdigit_Context
  CCtypeReqIsxdigit_Instance;

static const char * const CCtypeReqIsxdigit_PreDesc_Char[] = {
  "HexLetter",
  "Digit",
  "Other",
  "NA"
};

static const char * const * const CCtypeReqIsxdigit_PreDesc[] = {
  CCtypeReqIsxdigit_PreDesc_Char,
  NULL
};

static const int hex_letters[] = {
  'A', 'B', 'C', 'D', 'E', 'F',
  'a', 'b', 'c', 'd', 'e', 'f'
};

static const int digits[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

static const int others[] = {
  -1, '\0', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a',
  '\b', '\t', '\n', '\v', '\f', '\r', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
  '\x18', '\x19', '\x1a', '\e', '\x1c', '\x1d', '\x1e', '\x1f',
  ' ', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  ':', ';', '<', '=', '>', '?', '@',
  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '{', '|', '}', '~', '\x7f',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
  '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
  '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7',
  '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7',
  '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7',
  '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7',
  '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7',
  '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7',
  '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff'
};

static void CCtypeReqIsxdigit_Pre_Char_Prepare(
  CCtypeReqIsxdigit_Context *ctx,
  CCtypeReqIsxdigit_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIsxdigit_Pre_Char_HexLetter: {
      /*
       * While the character specified by `c` is a hexadecimal letter.
       */
      ctx->chars = hex_letters;
      ctx->chars_len = RTEMS_ARRAY_SIZE( hex_letters );
      break;
    }

    case CCtypeReqIsxdigit_Pre_Char_Digit: {
      /*
       * While the character specified by `c` is a digit.
       */
      ctx->chars = digits;
      ctx->chars_len = RTEMS_ARRAY_SIZE( digits );
      break;
    }

    case CCtypeReqIsxdigit_Pre_Char_Other: {
      /*
       * While the character specified by `c` is not a hexadecimal letter,
       * while the character specified by `c` is not a digit.
       */
      ctx->chars = others;
      ctx->chars_len = RTEMS_ARRAY_SIZE( others );
      break;
    }

    case CCtypeReqIsxdigit_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIsxdigit_Post_Result_Check(
  CCtypeReqIsxdigit_Context    *ctx,
  CCtypeReqIsxdigit_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsxdigit_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      size_t i;

      for ( i = 0; i < ctx->chars_len; ++i ) {
        T_ne_int( ctx->retval[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsxdigit_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      size_t i;

      for ( i = 0; i < ctx->chars_len; ++i ) {
        T_eq_int( ctx->retval[ i ], 0 );
      }
      break;
    }

    case CCtypeReqIsxdigit_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsxdigit_Action( CCtypeReqIsxdigit_Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->chars_len; ++i ) {
    ctx->retval[ i ] = isxdigit( ctx->chars[ i ] );
  }
}

static const CCtypeReqIsxdigit_Entry
CCtypeReqIsxdigit_Entries[] = {
  { 0, 0, CCtypeReqIsxdigit_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsxdigit_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsxdigit_Map[] = {
  0, 0, 1
};

static size_t CCtypeReqIsxdigit_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsxdigit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsxdigit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsxdigit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsxdigit_Scope,
  .initial_context = &CCtypeReqIsxdigit_Instance
};

static inline CCtypeReqIsxdigit_Entry CCtypeReqIsxdigit_PopEntry(
  CCtypeReqIsxdigit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsxdigit_Entries[
    CCtypeReqIsxdigit_Map[ index ]
  ];
}

static void CCtypeReqIsxdigit_TestVariant( CCtypeReqIsxdigit_Context *ctx )
{
  CCtypeReqIsxdigit_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsxdigit_Action( ctx );
  CCtypeReqIsxdigit_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsxdigit( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsxdigit, &CCtypeReqIsxdigit_Fixture )
{
  CCtypeReqIsxdigit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsxdigit_Pre_Char_HexLetter;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsxdigit_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsxdigit_PopEntry( ctx );
    CCtypeReqIsxdigit_TestVariant( ctx );
  }
}

/** @} */
