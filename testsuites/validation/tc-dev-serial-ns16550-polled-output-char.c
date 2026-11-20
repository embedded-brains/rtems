/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevSerialNs16550ReqPolledOutputChar
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

#include <libchip/ns16550.h>
#include <libchip/ns16550_p.h>
#include <rtems/bspIo.h>

#include <rtems/test.h>

/**
 * @defgroup DevSerialNs16550ReqPolledOutputChar \
 *   spec:/dev/serial/ns16550/req/polled-output-char
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_Char,
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_Negative,
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_Try,
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_Drain,
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_DrainTry,
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_NA
} DevSerialNs16550ReqPolledOutputChar_Pre_Request;

typedef enum {
  DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_Yes,
  DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_No,
  DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_NA
} DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty;

typedef enum {
  DevSerialNs16550ReqPolledOutputChar_Pre_Drained_Yes,
  DevSerialNs16550ReqPolledOutputChar_Pre_Drained_No,
  DevSerialNs16550ReqPolledOutputChar_Pre_Drained_NA
} DevSerialNs16550ReqPolledOutputChar_Pre_Drained;

typedef enum {
  DevSerialNs16550ReqPolledOutputChar_Post_Status_Zero,
  DevSerialNs16550ReqPolledOutputChar_Post_Status_MinusOne,
  DevSerialNs16550ReqPolledOutputChar_Post_Status_NA
} DevSerialNs16550ReqPolledOutputChar_Post_Status;

typedef enum {
  DevSerialNs16550ReqPolledOutputChar_Post_Action_Transmit,
  DevSerialNs16550ReqPolledOutputChar_Post_Action_WaitTransmit,
  DevSerialNs16550ReqPolledOutputChar_Post_Action_Wait,
  DevSerialNs16550ReqPolledOutputChar_Post_Action_None,
  DevSerialNs16550ReqPolledOutputChar_Post_Action_NA
} DevSerialNs16550ReqPolledOutputChar_Post_Action;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Request_NA : 1;
  uint16_t Pre_HoldingEmpty_NA : 1;
  uint16_t Pre_Drained_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Action : 3;
} DevSerialNs16550ReqPolledOutputChar_Entry;

/**
 * @brief Test context for spec:/dev/serial/ns16550/req/polled-output-char test
 *   case.
 */
typedef struct {
  /**
   * @brief This member provides the NS16550 device context.
   */
  ns16550_context dev;

  /**
   * @brief This member contains the value of the line status register.
   */
  uint8_t lsr;

  /**
   * @brief This member contains the count of line status register reads.
   */
  uint32_t status_reads;

  /**
   * @brief This member contains the count of transmitter holding register
   *   writes.
   */
  uint32_t thr_writes;

  /**
   * @brief This member contains the last value written to the transmitter
   *   holding register.
   */
  uint32_t thr;

  /**
   * @brief This member specifies the second parameter of the action.
   */
  int c;

  /**
   * @brief This member contains the expected value of the transmitter holding
   *   register.
   */
  uint32_t expected;

  /**
   * @brief This member contains the return value of the action.
   */
  int status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    DevSerialNs16550ReqPolledOutputChar_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} DevSerialNs16550ReqPolledOutputChar_Context;

static DevSerialNs16550ReqPolledOutputChar_Context
  DevSerialNs16550ReqPolledOutputChar_Instance;

static const char *const
  DevSerialNs16550ReqPolledOutputChar_PreDesc_Request[] =
    { "Char", "Negative", "Try", "Drain", "DrainTry", "NA" };

static const char *const
  DevSerialNs16550ReqPolledOutputChar_PreDesc_HoldingEmpty[] =
    { "Yes", "No", "NA" };

static const char *const
  DevSerialNs16550ReqPolledOutputChar_PreDesc_Drained[] =
    { "Yes", "No", "NA" };

static const char *const *const DevSerialNs16550ReqPolledOutputChar_PreDesc[] =
  { DevSerialNs16550ReqPolledOutputChar_PreDesc_Request,
    DevSerialNs16550ReqPolledOutputChar_PreDesc_HoldingEmpty,
    DevSerialNs16550ReqPolledOutputChar_PreDesc_Drained,
    NULL };

typedef DevSerialNs16550ReqPolledOutputChar_Context Context;

static uint8_t GetReg( uintptr_t port, uint8_t reg )
{
  Context *ctx;
  uint8_t  value;

  ctx = (Context *) port;

  if ( reg != NS16550_LINE_STATUS ) {
    return 0;
  }

  value = ctx->lsr;
  ++ctx->status_reads;

  /* The transmitter is empty after the first read */
  ctx->lsr = SP_LSR_THOLD | SP_LSR_TX;

  return value;
}

static void SetReg( uintptr_t port, uint8_t reg, uint8_t value )
{
  Context *ctx;

  ctx = (Context *) port;

  if ( reg == NS16550_TRANSMIT_BUFFER ) {
    ++ctx->thr_writes;
    ctx->thr = value;
  }
}

static void DevSerialNs16550ReqPolledOutputChar_Pre_Request_Prepare(
  DevSerialNs16550ReqPolledOutputChar_Context    *ctx,
  DevSerialNs16550ReqPolledOutputChar_Pre_Request state
)
{
  switch ( state ) {
    case DevSerialNs16550ReqPolledOutputChar_Pre_Request_Char: {
      /*
       * While the second parameter specifies a character and no flag.
       */
      ctx->c = 'c';
      ctx->expected = 'c';
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Request_Negative: {
      /*
       * While the second parameter is a negative value.
       */
      ctx->c = (signed char) 0xe9;
      ctx->expected = 0xe9;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Request_Try: {
      /*
       * While the second parameter specifies a character and
       * RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c = RTEMS_OUTPUT_CHAR_TRY | 'c';
      ctx->expected = 'c';
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Request_Drain: {
      /*
       * While the second parameter specifies RTEMS_OUTPUT_CHAR_DRAIN and not
       * RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c = RTEMS_OUTPUT_CHAR_DRAIN | 'c';
      ctx->expected = 0;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Request_DrainTry: {
      /*
       * While the second parameter specifies RTEMS_OUTPUT_CHAR_DRAIN and
       * RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c = RTEMS_OUTPUT_CHAR_DRAIN | RTEMS_OUTPUT_CHAR_TRY | 'c';
      ctx->expected = 0;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Request_NA:
      break;
  }
}

static void DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_Prepare(
  DevSerialNs16550ReqPolledOutputChar_Context         *ctx,
  DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty state
)
{
  switch ( state ) {
    case DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_Yes: {
      /*
       * While the NS16550 transmitter holding register is empty.
       */
      ctx->lsr |= SP_LSR_THOLD;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_No: {
      /*
       * While the NS16550 transmitter holding register is not empty.
       */
      ctx->lsr &= ~SP_LSR_THOLD;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_NA:
      break;
  }
}

static void DevSerialNs16550ReqPolledOutputChar_Pre_Drained_Prepare(
  DevSerialNs16550ReqPolledOutputChar_Context    *ctx,
  DevSerialNs16550ReqPolledOutputChar_Pre_Drained state
)
{
  switch ( state ) {
    case DevSerialNs16550ReqPolledOutputChar_Pre_Drained_Yes: {
      /*
       * While the NS16550 transmitter holding register and the transmitter
       * shift register are empty.
       */
      ctx->lsr |= SP_LSR_TX;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Drained_No: {
      /*
       * While the NS16550 transmitter holding register or the transmitter
       * shift register is not empty.
       */
      ctx->lsr &= ~SP_LSR_TX;
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Pre_Drained_NA:
      break;
  }
}

static void DevSerialNs16550ReqPolledOutputChar_Post_Status_Check(
  DevSerialNs16550ReqPolledOutputChar_Context    *ctx,
  DevSerialNs16550ReqPolledOutputChar_Post_Status state
)
{
  switch ( state ) {
    case DevSerialNs16550ReqPolledOutputChar_Post_Status_Zero: {
      /*
       * The return value of ns16550_polled_output_char() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Post_Status_MinusOne: {
      /*
       * The return value of ns16550_polled_output_char() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Post_Status_NA:
      break;
  }
}

static void DevSerialNs16550ReqPolledOutputChar_Post_Action_Check(
  DevSerialNs16550ReqPolledOutputChar_Context    *ctx,
  DevSerialNs16550ReqPolledOutputChar_Post_Action state
)
{
  switch ( state ) {
    case DevSerialNs16550ReqPolledOutputChar_Post_Action_Transmit: {
      /*
       * The ns16550_polled_output_char() function shall write the character
       * specified by the second parameter to the NS16550 transmitter holding
       * register without a wait.
       */
      T_eq_u32( ctx->status_reads, 1 );
      T_eq_u32( ctx->thr_writes, 1 );
      T_eq_u32( ctx->thr, ctx->expected );
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Post_Action_WaitTransmit: {
      /*
       * The ns16550_polled_output_char() function shall wait until the NS16550
       * transmitter holding register is empty and then write the character
       * specified by the second parameter to it.
       */
      T_gt_u32( ctx->status_reads, 1 );
      T_eq_u32( ctx->thr_writes, 1 );
      T_eq_u32( ctx->thr, ctx->expected );
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Post_Action_Wait: {
      /*
       * The ns16550_polled_output_char() function shall wait until the NS16550
       * transmitter is empty and write no character.
       */
      T_gt_u32( ctx->status_reads, 1 );
      T_eq_u32( ctx->thr_writes, 0 );
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Post_Action_None: {
      /*
       * The ns16550_polled_output_char() function shall neither wait nor write
       * a character.
       */
      T_eq_u32( ctx->status_reads, 1 );
      T_eq_u32( ctx->thr_writes, 0 );
      break;
    }

    case DevSerialNs16550ReqPolledOutputChar_Post_Action_NA:
      break;
  }
}

static void DevSerialNs16550ReqPolledOutputChar_Setup(
  DevSerialNs16550ReqPolledOutputChar_Context *ctx
)
{
  rtems_termios_device_context_initialize( &ctx->dev.base, "NS16550" );
  ctx->dev.port = (uintptr_t) ctx;
  ctx->dev.get_reg = GetReg;
  ctx->dev.set_reg = SetReg;
}

static void DevSerialNs16550ReqPolledOutputChar_Setup_Wrap( void *arg )
{
  DevSerialNs16550ReqPolledOutputChar_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  DevSerialNs16550ReqPolledOutputChar_Setup( ctx );
}

static void DevSerialNs16550ReqPolledOutputChar_Prepare(
  DevSerialNs16550ReqPolledOutputChar_Context *ctx
)
{
  ctx->lsr = 0;
  ctx->status_reads = 0;
  ctx->thr_writes = 0;
  ctx->thr = 0;
}

static void DevSerialNs16550ReqPolledOutputChar_Action(
  DevSerialNs16550ReqPolledOutputChar_Context *ctx
)
{
  ctx->status = ns16550_polled_output_char( &ctx->dev.base, ctx->c );
}

/* clang-format off */

static const DevSerialNs16550ReqPolledOutputChar_Entry
DevSerialNs16550ReqPolledOutputChar_Entries[] = {
  { 0, 0, 0, 0, DevSerialNs16550ReqPolledOutputChar_Post_Status_Zero,
    DevSerialNs16550ReqPolledOutputChar_Post_Action_Transmit },
  { 0, 0, 0, 0, DevSerialNs16550ReqPolledOutputChar_Post_Status_Zero,
    DevSerialNs16550ReqPolledOutputChar_Post_Action_WaitTransmit },
  { 0, 0, 0, 0, DevSerialNs16550ReqPolledOutputChar_Post_Status_MinusOne,
    DevSerialNs16550ReqPolledOutputChar_Post_Action_None },
  { 0, 0, 0, 0, DevSerialNs16550ReqPolledOutputChar_Post_Status_Zero,
    DevSerialNs16550ReqPolledOutputChar_Post_Action_None },
  { 0, 0, 0, 0, DevSerialNs16550ReqPolledOutputChar_Post_Status_Zero,
    DevSerialNs16550ReqPolledOutputChar_Post_Action_Wait }
};

static const uint8_t
DevSerialNs16550ReqPolledOutputChar_Map[] = {
  0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2, 3, 4, 3, 4, 3, 2, 3, 2
};

/* clang-format on */

static size_t DevSerialNs16550ReqPolledOutputChar_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  DevSerialNs16550ReqPolledOutputChar_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      DevSerialNs16550ReqPolledOutputChar_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture DevSerialNs16550ReqPolledOutputChar_Fixture = {
  .setup = DevSerialNs16550ReqPolledOutputChar_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = DevSerialNs16550ReqPolledOutputChar_Scope,
  .initial_context = &DevSerialNs16550ReqPolledOutputChar_Instance
};

static inline DevSerialNs16550ReqPolledOutputChar_Entry
DevSerialNs16550ReqPolledOutputChar_PopEntry(
  DevSerialNs16550ReqPolledOutputChar_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return DevSerialNs16550ReqPolledOutputChar_Entries
    [ DevSerialNs16550ReqPolledOutputChar_Map[ index ] ];
}

static void DevSerialNs16550ReqPolledOutputChar_TestVariant(
  DevSerialNs16550ReqPolledOutputChar_Context *ctx
)
{
  DevSerialNs16550ReqPolledOutputChar_Pre_Request_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  DevSerialNs16550ReqPolledOutputChar_Pre_Drained_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  DevSerialNs16550ReqPolledOutputChar_Action( ctx );
  DevSerialNs16550ReqPolledOutputChar_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  DevSerialNs16550ReqPolledOutputChar_Post_Action_Check(
    ctx,
    ctx->Map.entry.Post_Action
  );
}

/**
 * @fn void T_case_body_DevSerialNs16550ReqPolledOutputChar( void )
 */
T_TEST_CASE_FIXTURE(
  DevSerialNs16550ReqPolledOutputChar,
  &DevSerialNs16550ReqPolledOutputChar_Fixture
)
{
  DevSerialNs16550ReqPolledOutputChar_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = DevSerialNs16550ReqPolledOutputChar_Pre_Request_Char;
    ctx->Map.pcs[ 0 ] < DevSerialNs16550ReqPolledOutputChar_Pre_Request_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] =
        DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_Yes;
      ctx->Map.pcs[ 1 ] <
      DevSerialNs16550ReqPolledOutputChar_Pre_HoldingEmpty_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] =
          DevSerialNs16550ReqPolledOutputChar_Pre_Drained_Yes;
        ctx->Map.pcs[ 2 ] < DevSerialNs16550ReqPolledOutputChar_Pre_Drained_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = DevSerialNs16550ReqPolledOutputChar_PopEntry( ctx );
        DevSerialNs16550ReqPolledOutputChar_Prepare( ctx );
        DevSerialNs16550ReqPolledOutputChar_TestVariant( ctx );
      }
    }
  }
}

/** @} */
