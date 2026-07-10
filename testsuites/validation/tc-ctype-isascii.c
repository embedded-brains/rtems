/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CCtypeReqIsascii
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
 * @defgroup CCtypeReqIsascii spec:/c/ctype/req/isascii
 *
 * @{
 */

typedef enum {
  CCtypeReqIsascii_Pre_C_Ascii,
  CCtypeReqIsascii_Pre_C_NonAscii,
  CCtypeReqIsascii_Pre_C_NA
} CCtypeReqIsascii_Pre_C;

typedef enum {
  CCtypeReqIsascii_Post_Result_NonZero,
  CCtypeReqIsascii_Post_Result_Zero,
  CCtypeReqIsascii_Post_Result_NA
} CCtypeReqIsascii_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 2;
} CCtypeReqIsascii_Entry;

/**
 * @brief Test context for spec:/c/ctype/req/isascii test case.
 */
typedef struct {
  /**
   * @brief This member contains the test characters.
   */
  const int *chars;

  /**
   * @brief This member contains the number of test characters.
   */
  size_t n_chars;

  /**
   * @brief This member contains the return values.
   */
  int retvals[ 256 ];

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
    CCtypeReqIsascii_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CCtypeReqIsascii_Context;

static CCtypeReqIsascii_Context
  CCtypeReqIsascii_Instance;

static const char * const CCtypeReqIsascii_PreDesc_C[] = {
  "Ascii",
  "NonAscii",
  "NA"
};

static const char * const * const CCtypeReqIsascii_PreDesc[] = {
  CCtypeReqIsascii_PreDesc_C,
  NULL
};

static const int valid_chars[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
  80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
  116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
};

static const int invalid_chars[] = {
  -128, -127, -126, -125, -124, -123, -122, -121, -120, -119, -118, -117,
  -116, -115, -114, -113, -112, -111, -110, -109, -108, -107, -106, -105,
  -104, -103, -102, -101, -100, -99, -98, -97, -96, -95, -94, -93, -92,
  -91, -90, -89, -88, -87, -86, -85, -84, -83, -82, -81, -80, -79, -78,
  -77, -76, -75, -74, -73, -72, -71, -70, -69, -68, -67, -66, -65, -64,
  -63, -62, -61, -60, -59, -58, -57, -56, -55, -54, -53, -52, -51, -50,
  -49, -48, -47, -46, -45, -44, -43, -42, -41, -40, -39, -38, -37, -36,
  -35, -34, -33, -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22,
  -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7,
  -6, -5, -4, -3, -2, -1, 128, 129, 130, 131, 132, 133, 134, 135, 136,
  137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
  151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
  165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178,
  179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
  193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206,
  207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
  221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234,
  235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248,
  249, 250, 251, 252, 253, 254, 255
};

static void CCtypeReqIsascii_Pre_C_Prepare(
  CCtypeReqIsascii_Context *ctx,
  CCtypeReqIsascii_Pre_C    state
)
{
  switch ( state ) {
    case CCtypeReqIsascii_Pre_C_Ascii: {
      /*
       * While the character specified by `c` is greater than or equal to 0,
       * while the character specified by `c` is less than or equal to 127.
       */
      ctx->chars = valid_chars;
      ctx->n_chars = sizeof(valid_chars) / sizeof(valid_chars[0]);
      break;
    }

    case CCtypeReqIsascii_Pre_C_NonAscii: {
      /*
       * While the character specified by `c` is less than 0, or the character
       * specified by `c` is greater than 127.
       */
      ctx->chars = invalid_chars;
      ctx->n_chars = sizeof(invalid_chars) / sizeof(invalid_chars[0]);
      break;
    }

    case CCtypeReqIsascii_Pre_C_NA:
      break;
  }
}

static void CCtypeReqIsascii_Post_Result_Check(
  CCtypeReqIsascii_Context    *ctx,
  CCtypeReqIsascii_Post_Result state
)
{
  switch ( state ) {
    case CCtypeReqIsascii_Post_Result_NonZero: {
      /*
       * The return value of the function shall be non-zero.
       */
      for ( size_t i = 0; i < ctx->n_chars; ++i ) {
        T_ne_int( ctx->retvals[i], 0 );
      }
      break;
    }

    case CCtypeReqIsascii_Post_Result_Zero: {
      /*
       * The return value of the function shall be zero.
       */
      for ( size_t i = 0; i < ctx->n_chars; ++i ) {
        T_eq_int( ctx->retvals[i], 0 );
      }
      break;
    }

    case CCtypeReqIsascii_Post_Result_NA:
      break;
  }
}

static void CCtypeReqIsascii_Action( CCtypeReqIsascii_Context *ctx )
{
  for ( size_t i = 0; i < ctx->n_chars; ++i ) {
    ctx->retvals[i] = (isascii)( ctx->chars[i] );
  }
}

static const CCtypeReqIsascii_Entry
CCtypeReqIsascii_Entries[] = {
  { 0, 0, CCtypeReqIsascii_Post_Result_NonZero },
  { 0, 0, CCtypeReqIsascii_Post_Result_Zero }
};

static const uint8_t
CCtypeReqIsascii_Map[] = {
  0, 1
};

static size_t CCtypeReqIsascii_Scope( void *arg, char *buf, size_t n )
{
  CCtypeReqIsascii_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CCtypeReqIsascii_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CCtypeReqIsascii_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CCtypeReqIsascii_Scope,
  .initial_context = &CCtypeReqIsascii_Instance
};

static inline CCtypeReqIsascii_Entry CCtypeReqIsascii_PopEntry(
  CCtypeReqIsascii_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CCtypeReqIsascii_Entries[
    CCtypeReqIsascii_Map[ index ]
  ];
}

static void CCtypeReqIsascii_TestVariant( CCtypeReqIsascii_Context *ctx )
{
  CCtypeReqIsascii_Pre_C_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CCtypeReqIsascii_Action( ctx );
  CCtypeReqIsascii_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CCtypeReqIsascii( void )
 */
T_TEST_CASE_FIXTURE( CCtypeReqIsascii, &CCtypeReqIsascii_Fixture )
{
  CCtypeReqIsascii_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CCtypeReqIsascii_Pre_C_Ascii;
    ctx->Map.pcs[ 0 ] < CCtypeReqIsascii_Pre_C_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CCtypeReqIsascii_PopEntry( ctx );
    CCtypeReqIsascii_TestVariant( ctx );
  }
}

/** @} */
