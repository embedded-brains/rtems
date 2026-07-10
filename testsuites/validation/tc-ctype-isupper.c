/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsupper
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
 * @defgroup CCtypeReqIsupper spec:/c/ctype/req/isupper
 *
 * @{
 */

typedef enum {
  CCtypeReqIsupper_Pre_Char_Upper,
  CCtypeReqIsupper_Pre_Char_Lower,
  CCtypeReqIsupper_Pre_Char_Digit,
  CCtypeReqIsupper_Pre_Char_Control,
  CCtypeReqIsupper_Pre_Char_Punct,
  CCtypeReqIsupper_Pre_Char_Space,
  CCtypeReqIsupper_Pre_Char_Other,
  CCtypeReqIsupper_Pre_Char_EOF,
  CCtypeReqIsupper_Pre_Char_NA
} CCtypeReqIsupper_Pre_Char;

typedef enum {
  CCtypeReqIsupper_Post_Result_NonZero,
  CCtypeReqIsupper_Post_Result_Zero,
  CCtypeReqIsupper_Post_Result_NA
} CCtypeReqIsupper_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Char_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsupper_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isupper test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `c` parameter value.
   */
  int c;

  /**
   * @brief This member points to the array of characters to test.
   */
  const int *c_array;

  /**
   * @brief This member specifies the number of characters to test.
   */
  size_t c_count;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the accumulated nonzero results.
   */
  bool all_nonzero;

  /**
   * @brief This member contains the accumulated zero results.
   */
  bool all_zero;

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
    CCtypeReqIsupper_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsupper_Context;

static CCtypeReqIsupper_Context
  CCtypeReqIsupper_Instance;

static const char * const CCtypeReqIsupper_PreDesc_Char[] = {
  "Upper",
  "Lower",
  "Digit",
  "Control",
  "Punct",
  "Space",
  "Other",
  "EOF",
  "NA"
};

static const char * const * const CCtypeReqIsupper_PreDesc[] = {
  CCtypeReqIsupper_PreDesc_Char,
  NULL
};

static const int upper_chars[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};
static const int lower_chars[] = {
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};
static const int digit_chars[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
static const int space_chars[] = {
  '\t', '\n', '\v', '\f', '\r', ' '
};
static const int punct_chars[] = {
  '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'
};
static const int control_chars[] = {
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a', '\b', '\x0e', '\x0f', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', '\x7f'
};
static const int other_chars[] = {
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f', '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f', '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf', '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf', '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf', '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf', '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef', '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff'
};

static void CCtypeReqIsupper_Pre_Char_Prepare(
  CCtypeReqIsupper_Context *ctx,
  CCtypeReqIsupper_Pre_Char state
)
{
  switch ( state ) {
    case CCtypeReqIsupper_Pre_Char_Upper: {
      /*
       * While the character specified by `c` is an uppercase letter.
       */
      ctx->c_array = upper_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( upper_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_Lower: {
      /*
       * While the character specified by `c` is a lowercase letter.
       */
      ctx->c_array = lower_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( lower_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_Digit: {
      /*
       * While the character specified by `c` is a digit.
       */
      ctx->c_array = digit_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( digit_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_Control: {
      /*
       * While the character specified by `c` is a control character.
       */
      ctx->c_array = control_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( control_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_Punct: {
      /*
       * While the character specified by `c` is a punctuation character.
       */
      ctx->c_array = punct_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( punct_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_Space: {
      /*
       * While the character specified by `c` is a space character.
       */
      ctx->c_array = space_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( space_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_Other: {
      /*
       * While the character specified by `c` is another character.
       */
      ctx->c_array = other_chars;
      ctx->c_count = RTEMS_ARRAY_SIZE( other_chars );
      break;
    }

    case CCtypeReqIsupper_Pre_Char_EOF: {
      /*
       * While the character specified by `c` is equal to EOF.
       */
      ctx->c_count = 0;
      ctx->c = EOF;
      break;
    }

    case CCtypeReqIsupper_Pre_Char_NA:
      break;
  }
}

static void CCtypeReqIsupper_Post_Result_Check(
  CCtypeReqIsupper_Context    *ctx,
  CCtypeReqIsupper_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsupper_Post_Result_NonZero: {
      /*
       * The return value shall be non-zero.
       */
      T_true( ctx->all_nonzero );
      T_false( ctx->all_zero );
      break;
    }

    case CCtypeReqIsupper_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      T_true( ctx->all_zero );
      T_false( ctx->all_nonzero );
      break;
    }

    case CCtypeReqIsupper_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsupper_Action( CCtypeReqIsupper_Context *ctx )
{
  size_t i;

  ctx->all_nonzero = true;
  ctx->all_zero = true;

  if ( ctx->c_count > 0 ) {
    for ( i = 0; i < ctx->c_count; ++i ) {
      ctx->c = ctx->c_array[ i ];
      ctx->retval = isupper( ctx->c );

      if ( ctx->retval == 0 ) {
        ctx->all_nonzero = false;
      } else {
        ctx->all_zero = false;
      }
    }
  } else {
    ctx->retval = isupper( ctx->c );

    if ( ctx->retval == 0 ) {
      ctx->all_nonzero = false;
    } else {
      ctx->all_zero = false;
    }
  }
}

static const CCtypeReqIsupper_Entry
CCtypeReqIsupper_Entries[] = {
  { 0, 0, CCtypeReqIsupper_Post_Result_Zero },
  { 0, 0, CCtypeReqIsupper_Post_Result_NonZero }
};

static const uint8_t
CCtypeReqIsupper_Map[] = {
  1, 0, 0, 0, 0, 0, 0, 0
};

static size_t CCtypeReqIsupper_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsupper_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsupper_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsupper_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsupper_Scope,
  .initial_context = &CCtypeReqIsupper_Instance
};

static inline CCtypeReqIsupper_Entry CCtypeReqIsupper_PopEntry(
  CCtypeReqIsupper_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsupper_Entries[
    CCtypeReqIsupper_Map[ index ]
  ];
}

static void CCtypeReqIsupper_TestVariant( CCtypeReqIsupper_Context *ctx )
{
  CCtypeReqIsupper_Pre_Char_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsupper_Action( ctx );
  CCtypeReqIsupper_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsupper( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsupper, &CCtypeReqIsupper_Fixture )
{
  CCtypeReqIsupper_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsupper_Pre_Char_Upper;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsupper_Pre_Char_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsupper_PopEntry( ctx );
    CCtypeReqIsupper_TestVariant( ctx );
  }
}

/** @} */
