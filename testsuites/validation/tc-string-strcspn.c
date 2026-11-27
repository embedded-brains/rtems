/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrcspn
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrcspn spec:/c/string/req/strcspn
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The string referenced by the `s2` parameter is followed by a character of
 * the segment. A segment of a proper prefix or of the whole string holds three
 * characters. None of them occurs in the string referenced by the `s2`
 * parameter. The last character of the string referenced by the `s2` parameter
 * ends a segment which is empty or a proper prefix. A character of the segment
 * follows the end of the string. A character of the segment differs from the
 * character 0xe2 in the most significant bit only. The test uses it where the
 * string referenced by the `s2` parameter holds a character greater than 127.
 *
 * @{
 */

typedef enum {
  CStringReqStrcspn_Pre_Reject_Empty,
  CStringReqStrcspn_Pre_Reject_NonEmpty,
  CStringReqStrcspn_Pre_Reject_NA
} CStringReqStrcspn_Pre_Reject;

typedef enum {
  CStringReqStrcspn_Pre_HighByte_Yes,
  CStringReqStrcspn_Pre_HighByte_No,
  CStringReqStrcspn_Pre_HighByte_NA
} CStringReqStrcspn_Pre_HighByte;

typedef enum {
  CStringReqStrcspn_Pre_Segment_Empty,
  CStringReqStrcspn_Pre_Segment_None,
  CStringReqStrcspn_Pre_Segment_Part,
  CStringReqStrcspn_Pre_Segment_Whole,
  CStringReqStrcspn_Pre_Segment_NA
} CStringReqStrcspn_Pre_Segment;

typedef enum {
  CStringReqStrcspn_Post_Result_Zero,
  CStringReqStrcspn_Post_Result_Index,
  CStringReqStrcspn_Post_Result_Length,
  CStringReqStrcspn_Post_Result_NA
} CStringReqStrcspn_Post_Result;

typedef enum {
  CStringReqStrcspn_Post_S1_Nop,
  CStringReqStrcspn_Post_S1_NA
} CStringReqStrcspn_Post_S1;

typedef enum {
  CStringReqStrcspn_Post_S2_Nop,
  CStringReqStrcspn_Post_S2_NA
} CStringReqStrcspn_Post_S2;

typedef enum {
  CStringReqStrcspn_Post_Errno_Nop,
  CStringReqStrcspn_Post_Errno_NA
} CStringReqStrcspn_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Reject_NA : 1;
  uint16_t Pre_HighByte_NA : 1;
  uint16_t Pre_Segment_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringReqStrcspn_Entry;

typedef enum {
  SEGMENT_EMPTY,
  SEGMENT_NONE,
  SEGMENT_PART,
  SEGMENT_WHOLE
} Segment;

/**
 * @brief Test context for spec:/c/string/req/strcspn test case.
 */
typedef struct {
  /**
   * @brief This member specifies the length of the string referenced by the
   *   `s2` parameter.
   */
  size_t reject_length;

  /**
   * @brief This member is true, if the string referenced by the `s2` parameter
   *   holds a character with a value greater than 127.
   */
  bool high;

  /**
   * @brief This member specifies the last character of the string referenced
   *   by the `s2` parameter, if the member high is true.
   */
  unsigned char high_byte;

  /**
   * @brief This member specifies the maximum initial segment.
   */
  Segment segment;

  /**
   * @brief This member contains the index of the first character of the string
   *   referenced by the `s1` parameter which occurs in the string referenced
   *   by the `s2` parameter.
   */
  size_t stop;

  /**
   * @brief This member contains the length of the string referenced by the
   *   `s1` parameter.
   */
  size_t length;

  /**
   * @brief This member provides the string referenced by the `s1` parameter.
   */
  unsigned char s1_buf[ 16 ];

  /**
   * @brief This member provides the string referenced by the `s2` parameter.
   */
  unsigned char s2_buf[ 8 ];

  /**
   * @brief This member contains the content of the memory which holds the
   *   string referenced by the `s1` parameter before the strcspn() call.
   */
  unsigned char s1_before[ 16 ];

  /**
   * @brief This member contains the content of the memory which holds the
   *   string referenced by the `s2` parameter before the strcspn() call.
   */
  unsigned char s2_before[ 8 ];

  /**
   * @brief This member contains the return value of the strcspn() call.
   */
  size_t retval;

  /**
   * @brief This member contains the value of errno after the strcspn() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

    /**
     * @brief This member defines the representative index for the next action.
     */
    size_t rep;

    /**
     * @brief If this member is true, then the current run of the
     *   representatives should be skipped.
     */
    bool skip_rep;

    /**
     * @brief This member records the representatives which ran.
     */
    bool rep_ran[ 15 ];

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
    CStringReqStrcspn_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrcspn_Context;

static CStringReqStrcspn_Context CStringReqStrcspn_Instance;

static const char *const CStringReqStrcspn_PreDesc_Reject[] =
  { "Empty", "NonEmptyOne", "NonEmptyTwo", "NonEmptyThree", "NA" };

static const char *const CStringReqStrcspn_PreDesc_HighByte[] =
  { "YesMin", "YesMid", "YesMax", "No", "NA" };

static const char *const CStringReqStrcspn_PreDesc_Segment[] =
  { "Empty", "None", "Part", "Whole", "NA" };

static const char *const *const CStringReqStrcspn_PreDesc[] = {
  CStringReqStrcspn_PreDesc_Reject,
  CStringReqStrcspn_PreDesc_HighByte,
  CStringReqStrcspn_PreDesc_Segment,
  NULL
};

typedef enum {
  CStringReqStrcspn_Pre_Reject_NonEmpty_One,
  CStringReqStrcspn_Pre_Reject_NonEmpty_Two,
  CStringReqStrcspn_Pre_Reject_NonEmpty_Three
} CStringReqStrcspn_Pre_Reject_NonEmpty_Rep;

typedef enum {
  CStringReqStrcspn_Pre_HighByte_Yes_Min,
  CStringReqStrcspn_Pre_HighByte_Yes_Mid,
  CStringReqStrcspn_Pre_HighByte_Yes_Max
} CStringReqStrcspn_Pre_HighByte_Yes_Rep;

static const uint8_t CStringReqStrcspn_PreRepOffset_Reject[] = { 0, 1, 4, 5 };

static const uint8_t CStringReqStrcspn_PreRepOffset_HighByte[] =
  { 0, 3, 4, 5 };

static const uint8_t CStringReqStrcspn_PreRepOffset_Segment[] =
  { 0, 1, 2, 3, 4, 5 };

static const uint8_t *const CStringReqStrcspn_PreRepOffset[] = {
  CStringReqStrcspn_PreRepOffset_Reject,
  CStringReqStrcspn_PreRepOffset_HighByte,
  CStringReqStrcspn_PreRepOffset_Segment
};

static size_t CStringReqStrcspn_GetRepresentativeCountOf(
  const CStringReqStrcspn_Context *ctx,
  size_t                           index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringReqStrcspn_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringReqStrcspn_GetRepresentative(
  const CStringReqStrcspn_Context *ctx,
  size_t                           index
)
{
  size_t count;

  count = CStringReqStrcspn_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringReqStrcspn_GetRepresentativeCount(
  const CStringReqStrcspn_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringReqStrcspn_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringReqStrcspn_Pre_Reject_NonEmpty_Rep
CStringReqStrcspn_Pre_Reject_NonEmpty_GetRepresentative(
  const CStringReqStrcspn_Context *ctx
)
{
  return (
    CStringReqStrcspn_Pre_Reject_NonEmpty_Rep
  ) CStringReqStrcspn_GetRepresentative( ctx, 0 );
}

static CStringReqStrcspn_Pre_HighByte_Yes_Rep
CStringReqStrcspn_Pre_HighByte_Yes_GetRepresentative(
  const CStringReqStrcspn_Context *ctx
)
{
  return (
    CStringReqStrcspn_Pre_HighByte_Yes_Rep
  ) CStringReqStrcspn_GetRepresentative( ctx, 1 );
}

static const uint8_t CStringReqStrcspn_PreRepBase[] = { 0, 5, 10 };

static const uint8_t CStringReqStrcspn_PreRepStates[] = { 2, 2, 4 };

static void CStringReqStrcspn_ResetRepresentatives(
  CStringReqStrcspn_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 15; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringReqStrcspn_MarkRepresentatives(
  CStringReqStrcspn_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = CStringReqStrcspn_PreRepBase[ i ] +
          CStringReqStrcspn_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringReqStrcspn_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringReqStrcspn_CheckRepresentatives(
  const CStringReqStrcspn_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringReqStrcspn_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringReqStrcspn_PreRepBase[ i ] ];

    for ( state = 0; state < CStringReqStrcspn_PreRepStates[ i ]; ++state ) {
      size_t k;
      bool   any;

      any = false;

      for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
        any = any || ran[ k ];
      }

      if ( any ) {
        for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
          T_quiet_true(
            ran[ k ],
            "representative %s did not run",
            CStringReqStrcspn_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef CStringReqStrcspn_Context Context;

#define REJECT_MAX 3

#define SEGMENT_LENGTH 3

static const unsigned char reject_members[ REJECT_MAX ] = { 'a', 'c', 'e' };

static const unsigned char segment_low[ SEGMENT_LENGTH ] = { 'b', 'd', 'f' };

static const unsigned char segment_high[ SEGMENT_LENGTH ] = { 0x62, 'd', 'f' };

static void CheckUnchanged(
  const unsigned char *actual,
  const unsigned char *expected,
  size_t               size
)
{
  size_t i;

  for ( i = 0; i < size; ++i ) {
    T_quiet_eq_u8( actual[ i ], expected[ i ] );
  }
}

static void Build( Context *ctx )
{
  const unsigned char *segment;
  unsigned char        stop;
  size_t               i;
  size_t               n;

  for ( i = 0; i < ctx->reject_length; ++i ) {
    ctx->s2_buf[ i ] = reject_members[ i ];
  }

  if ( ctx->high ) {
    ctx->s2_buf[ ctx->reject_length - 1 ] = ctx->high_byte;
    segment = segment_high;
  } else {
    segment = segment_low;
  }

  if ( ctx->reject_length > 0 ) {
    stop = ctx->s2_buf[ ctx->reject_length - 1 ];
  } else {
    stop = 0;
  }

  ctx->s2_buf[ ctx->reject_length + 1 ] = segment[ 0 ];
  n = 0;

  switch ( ctx->segment ) {
    case SEGMENT_NONE:
      ctx->s1_buf[ n ] = stop;
      ++n;
      ctx->s1_buf[ n ] = segment[ 0 ];
      ++n;
      break;
    case SEGMENT_PART:
    case SEGMENT_WHOLE:
      for ( i = 0; i < SEGMENT_LENGTH; ++i ) {
        ctx->s1_buf[ n ] = segment[ i ];
        ++n;
      }

      ctx->stop = n;

      if ( ctx->segment == SEGMENT_PART ) {
        ctx->s1_buf[ n ] = stop;
        ++n;
        ctx->s1_buf[ n ] = segment[ 0 ];
        ++n;
      }

      break;
    default:
      break;
  }

  ctx->length = n;
  ctx->s1_buf[ n + 1 ] = segment[ 0 ];
}

static void CStringReqStrcspn_Pre_Reject_Prepare(
  CStringReqStrcspn_Context   *ctx,
  CStringReqStrcspn_Pre_Reject state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_Reject_Empty: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * equal to zero.
       */
      ctx->reject_length = 0;
      break;
    }

    case CStringReqStrcspn_Pre_Reject_NonEmpty: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * greater than zero.
       */
      switch (
        CStringReqStrcspn_Pre_Reject_NonEmpty_GetRepresentative( ctx )
      ) {
        case CStringReqStrcspn_Pre_Reject_NonEmpty_One: {
          ctx->reject_length = 1;
          break;
        }

        case CStringReqStrcspn_Pre_Reject_NonEmpty_Two: {
          ctx->reject_length = 2;
          break;
        }

        case CStringReqStrcspn_Pre_Reject_NonEmpty_Three: {
          ctx->reject_length = REJECT_MAX;
          break;
        }
      }
      break;
    }

    case CStringReqStrcspn_Pre_Reject_NA:
      break;
  }
}

static void CStringReqStrcspn_Pre_HighByte_Prepare(
  CStringReqStrcspn_Context     *ctx,
  CStringReqStrcspn_Pre_HighByte state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_HighByte_Yes: {
      /*
       * While the string referenced by the `s2` parameter holds a character
       * with a value greater than 127, where the value is interpreted as type
       * `unsigned char`.
       */
      switch ( CStringReqStrcspn_Pre_HighByte_Yes_GetRepresentative( ctx ) ) {
        case CStringReqStrcspn_Pre_HighByte_Yes_Min: {
          ctx->high_byte = 0x80;
          break;
        }

        case CStringReqStrcspn_Pre_HighByte_Yes_Mid: {
          ctx->high_byte = 0xe2;
          break;
        }

        case CStringReqStrcspn_Pre_HighByte_Yes_Max: {
          ctx->high_byte = 0xff;
          break;
        }
      }

      ctx->high = true;
      break;
    }

    case CStringReqStrcspn_Pre_HighByte_No: {
      /*
       * While the string referenced by the `s2` parameter holds no character
       * with a value greater than 127, where the value is interpreted as type
       * `unsigned char`.
       */
      ctx->high = false;
      break;
    }

    case CStringReqStrcspn_Pre_HighByte_NA:
      break;
  }
}

static void CStringReqStrcspn_Pre_Segment_Prepare(
  CStringReqStrcspn_Context    *ctx,
  CStringReqStrcspn_Pre_Segment state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_Segment_Empty: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * equal to zero.
       */
      ctx->segment = SEGMENT_EMPTY;
      break;
    }

    case CStringReqStrcspn_Pre_Segment_None: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * greater than zero, while the first character of the string referenced
       * by the `s1` parameter occurs in the string referenced by the `s2`
       * parameter.
       */
      ctx->segment = SEGMENT_NONE;
      break;
    }

    case CStringReqStrcspn_Pre_Segment_Part: {
      /*
       * While the first character of the string referenced by the `s1`
       * parameter does not occur in the string referenced by the `s2`
       * parameter, while a character of the string referenced by the `s1`
       * parameter occurs in the string referenced by the `s2` parameter.
       */
      ctx->segment = SEGMENT_PART;
      break;
    }

    case CStringReqStrcspn_Pre_Segment_Whole: {
      /*
       * While the length of the string referenced by the `s1` parameter is
       * greater than zero, while no character of the string referenced by the
       * `s1` parameter occurs in the string referenced by the `s2` parameter.
       */
      ctx->segment = SEGMENT_WHOLE;
      break;
    }

    case CStringReqStrcspn_Pre_Segment_NA:
      break;
  }
}

static void CStringReqStrcspn_Post_Result_Check(
  CStringReqStrcspn_Context    *ctx,
  CStringReqStrcspn_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Post_Result_Zero: {
      /*
       * The return value of strcspn() shall be equal to zero.
       */
      T_eq_sz( ctx->retval, 0 );
      break;
    }

    case CStringReqStrcspn_Post_Result_Index: {
      /*
       * The return value of strcspn() shall be equal to the index of the first
       * character of the string referenced by the `s1` parameter which occurs
       * in the string referenced by the `s2` parameter.
       */
      T_eq_sz( ctx->retval, ctx->stop );
      break;
    }

    case CStringReqStrcspn_Post_Result_Length: {
      /*
       * The return value of strcspn() shall be equal to the length of the
       * string referenced by the `s1` parameter.
       */
      T_eq_sz( ctx->retval, ctx->length );
      break;
    }

    case CStringReqStrcspn_Post_Result_NA:
      break;
  }
}

static void CStringReqStrcspn_Post_S1_Check(
  CStringReqStrcspn_Context *ctx,
  CStringReqStrcspn_Post_S1  state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Post_S1_Nop: {
      /*
       * The bytes of the string referenced by the `s1` parameter shall not be
       * modified.
       */
      CheckUnchanged( ctx->s1_buf, ctx->s1_before, sizeof( ctx->s1_buf ) );
      break;
    }

    case CStringReqStrcspn_Post_S1_NA:
      break;
  }
}

static void CStringReqStrcspn_Post_S2_Check(
  CStringReqStrcspn_Context *ctx,
  CStringReqStrcspn_Post_S2  state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Post_S2_Nop: {
      /*
       * The bytes of the string referenced by the `s2` parameter shall not be
       * modified.
       */
      CheckUnchanged( ctx->s2_buf, ctx->s2_before, sizeof( ctx->s2_buf ) );
      break;
    }

    case CStringReqStrcspn_Post_S2_NA:
      break;
  }
}

static void CStringReqStrcspn_Post_Errno_Check(
  CStringReqStrcspn_Context   *ctx,
  CStringReqStrcspn_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqStrcspn_Post_Errno_NA:
      break;
  }
}

static void CStringReqStrcspn_Prepare( CStringReqStrcspn_Context *ctx )
{
  memset( ctx->s1_buf, 0, sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 0, sizeof( ctx->s2_buf ) );
  ctx->stop = 0;
  ctx->length = 0;
  ctx->retval = SIZE_MAX;
}

static void CStringReqStrcspn_Action( CStringReqStrcspn_Context *ctx )
{
  Build( ctx );
  memcpy( ctx->s1_before, ctx->s1_buf, sizeof( ctx->s1_buf ) );
  memcpy( ctx->s2_before, ctx->s2_buf, sizeof( ctx->s2_buf ) );
  errno = -1;
  ctx->retval = strcspn(
    (const char *) ctx->s1_buf,
    (const char *) ctx->s2_buf
  );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqStrcspn_Entry
CStringReqStrcspn_Entries[] = {
  { 1, 0, 0, 0, CStringReqStrcspn_Post_Result_NA, CStringReqStrcspn_Post_S1_NA,
    CStringReqStrcspn_Post_S2_NA, CStringReqStrcspn_Post_Errno_NA },
  { 0, 0, 0, 0, CStringReqStrcspn_Post_Result_Zero,
    CStringReqStrcspn_Post_S1_Nop, CStringReqStrcspn_Post_S2_Nop,
    CStringReqStrcspn_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringReqStrcspn_Post_Result_Length,
    CStringReqStrcspn_Post_S1_Nop, CStringReqStrcspn_Post_S2_Nop,
    CStringReqStrcspn_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringReqStrcspn_Post_Result_Index,
    CStringReqStrcspn_Post_S1_Nop, CStringReqStrcspn_Post_S2_Nop,
    CStringReqStrcspn_Post_Errno_Nop }
};

static const uint8_t
CStringReqStrcspn_Map[] = {
  0, 0, 0, 0, 1, 0, 0, 2, 1, 1, 3, 2, 1, 1, 3, 2
};

/* clang-format on */

static size_t CStringReqStrcspn_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrcspn_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = CStringReqStrcspn_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CStringReqStrcspn_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringReqStrcspn_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CStringReqStrcspn_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrcspn_Scope,
  .initial_context = &CStringReqStrcspn_Instance
};

static inline CStringReqStrcspn_Entry CStringReqStrcspn_PopEntry(
  CStringReqStrcspn_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrcspn_Entries[ CStringReqStrcspn_Map[ index ] ];
}

static void CStringReqStrcspn_TestVariant( CStringReqStrcspn_Context *ctx )
{
  CStringReqStrcspn_Pre_Reject_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrcspn_Pre_HighByte_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrcspn_Pre_Segment_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringReqStrcspn_Action( ctx );
  CStringReqStrcspn_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrcspn_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringReqStrcspn_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringReqStrcspn_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqStrcspn( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrcspn, &CStringReqStrcspn_Fixture )
{
  CStringReqStrcspn_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CStringReqStrcspn_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrcspn_Pre_Reject_Empty;
    ctx->Map.pcs[ 0 ] < CStringReqStrcspn_Pre_Reject_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqStrcspn_Pre_HighByte_Yes;
      ctx->Map.pcs[ 1 ] < CStringReqStrcspn_Pre_HighByte_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqStrcspn_Pre_Segment_Empty;
        ctx->Map.pcs[ 2 ] < CStringReqStrcspn_Pre_Segment_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStringReqStrcspn_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep < CStringReqStrcspn_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          CStringReqStrcspn_Prepare( ctx );
          CStringReqStrcspn_TestVariant( ctx );

          CStringReqStrcspn_MarkRepresentatives( ctx );
        }
      }
    }
  }

  CStringReqStrcspn_CheckRepresentatives( ctx );
}

/** @} */
