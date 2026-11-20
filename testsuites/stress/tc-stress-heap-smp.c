/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocValStressHeapSmp
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

#include <inttypes.h>
#include <rtems.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <rtems/malloc.h>
#include <sys/cpuset.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocValStressHeapSmp spec:/rtems/malloc/val/stress-heap-smp
 *
 * @ingroup TestsuitesStressHeapSmp
 *
 * @brief Stresses the heap which serves malloc() with a randomized load on
 *   each processor.
 *
 * A heap which serves several processors can fail in ways which a test on one
 * processor never shows. A block can lose its contents, a free block can get
 * lost, or one processor can starve while the others hold the lock of the
 * heap. This test loads the heap which serves malloc() from every processor at
 * once and drives it into fragmentation. It checks that the heap stays
 * consistent under this load. It also records how fragmented the heap gets and
 * how long each call takes, so that you can compare targets and versions.
 *
 * The test runs one worker on each processor, up to 32 processors. Each worker
 * is pinned to its processor and runs at priority 2, so the workers compete
 * for the heap and not for the processors. The test divides the time into
 * periods of 100 ms. After a warm-up of 10 periods, the load lasts 180 s.
 *
 * To keep the heap under pressure, each worker allocates while the bytes it
 * holds stay below its target, and it frees a block otherwise. The target
 * follows a sine curve with a phase and a frequency of its own. The sum of the
 * targets of all workers lies between 0.6 and 1.1 times the heap size, so the
 * heap runs full in each period and allocations fail. After a failed
 * allocation, the worker frees a block, so that it keeps making progress. A
 * greedy allocation restricts the heap to 4 MiB, which makes the heap run full
 * on every target.
 *
 * A full heap alone does not fragment, so three properties of the load break
 * up the free memory. First, one block of eight lives for 10 to 100 periods,
 * while the other blocks live for a short time. The long-lived blocks
 * therefore stay scattered between the free areas. The long-lived blocks of a
 * worker hold at most half of its share of the heap, so a worker above its
 * target always has a short-lived block to free. Second, a sine curve with a
 * period of 2 s, shared by all workers, moves the block sizes between small
 * blocks of 8 to 255 bytes and large blocks of 1 KiB to 64 KiB. Third, one
 * allocation of eight calls posix_memalign() with an alignment of 8 to 4096
 * bytes.
 *
 * The heap must also handle a block which returns from another processor. A
 * worker therefore hands a block to its neighbour with the probability 1/8
 * through a lock-free ring, and the neighbour frees it.
 *
 * Each worker fills a block with a pattern of its owner, its sequence number
 * and its size. It checks the pattern before it frees the block, so a block
 * which the heap gave out twice or overwrote shows up as a pattern error. A
 * worker which does not complete an allocation and a free in a period counts a
 * progress error. At the end, the workers free all their blocks. The heap walk
 * then has to report no error, and the free size has to equal the free size
 * before the run. A failed allocation is no error, because the test exhausts
 * the heap on purpose.
 *
 * Once per second, the runner pauses all workers and samples the free size and
 * the largest free block of the heap. These samples show how the fragmentation
 * develops over the run. The test checks no limit for the fragmentation or for
 * the execution times. These figures depend on the target, the processor count
 * and the allocator, so the test prints them for a comparison of runs.
 *
 * For example, a run on a T4240 with 24 processors made 1064506 successful and
 * 294662 failed allocations. Each failed allocation was a fragmentation
 * failure, and the median fragmentation of the 180 samples was 0.998. The test
 * printed 629 us as the median execution time of malloc(), so the true median
 * lies between 629 us and 1258 us. Nearly every call waited for the lock of
 * the heap, which the 24 workers share. Each worker made between 43946 and
 * 44592 successful allocations, so the lock served the workers evenly.
 *
 * The fragmentation of a sample is 1 minus the ratio of the largest free block
 * to the free size. The test prints it in parts per million. A value near 1
 * million means that the free memory consists of small pieces. A failed
 * allocation counts as a fragmentation failure, if the free size was at least
 * the requested size plus the alignment. Such a failure stems from
 * fragmentation and not from a lack of memory.
 *
 * The execution times of malloc() and free() go into histograms with one bin
 * per power of two of CPU counter ticks. Bin i holds the times from 2^(i - 1)
 * ticks up to below 2^i ticks. The printed median is the lower bound of the
 * bin which holds the median, limited to the range of the values. The true
 * median therefore lies between the printed value and twice that value. The
 * output gives every figure in JSON between the lines "\*\*\* BEGIN OF JSON
 * ***" and "*** END OF JSON \*\*\*".
 *
 * The test suite of this test case exists only in an SMP build without the
 * pre- qualified configuration. The build option BUILD_STRESS_TESTS enables
 * it. The option is off by default, because a run takes more than three
 * minutes.
 *
 * This test case performs the following actions:
 *
 * - Start one worker on each processor. Let the workers allocate and free
 *   blocks for the warm-up periods and for the load time. Pause the workers
 *   once per second to sample the heap. Stop the workers while they hold their
 *   blocks.
 *
 *   - Check that each block which a worker allocated meets the requested
 *     alignment.
 *
 *   - Check that each worker completed at least one allocation and one free in
 *     each period of the load time.
 *
 * - Get the statistics of the heap and the size of the largest free block
 *   while the workers hold their blocks.
 *
 *   - Check that the largest free block is not larger than the free size.
 *
 * - Let the workers free all their blocks. Walk the heap. Get the statistics
 *   of the heap.
 *
 *   - Check that each block carried the fill pattern of its owner, its
 *     sequence number and its size when it was freed.
 *
 *   - Check that the heap walk reported no error.
 *
 *   - Check that the free size after the run is equal to the free size before
 *     the run.
 *
 * - Print the figures of the workers and of the heap.
 *
 * @{
 */

#define WORKER_MAX 32

#define HEAP_SIZE ( 4 * 1024 * 1024 )

#define LOAD_SECONDS 180

#define PERIODS_PER_SECOND 10

#define WARM_UP_PERIODS 10

#define PERIOD_MAX ( ( LOAD_SECONDS + 10 ) * PERIODS_PER_SECOND )

#define SAMPLE_MAX ( LOAD_SECONDS + 1 )

/* The size sine has a period of two seconds. */
#define SIZE_PERIODS 20

#define SMALL_OCTAVE_MIN 3

#define SMALL_OCTAVE_COUNT 5

#define LARGE_OCTAVE_MIN 10

#define LARGE_OCTAVE_COUNT 6

/* The least mean block size of DrawSize() is above this value. */
#define SIZE_MEAN_MIN 64

#define LONG_PERIODS_MIN 10

#define LONG_PERIODS_SPAN 91

/* The base and the amplitude of a target as a fraction of 65536. */
#define TARGET_BASE 55706

#define TARGET_AMPLITUDE 16384

#define SINE_ONE 32767

#define RING_SIZE 64

#define SINE_SIZE 1024

#define HISTOGRAM_SIZE 32

#define PI 3.14159265358979323846

#define CACHE_LINE_ALIGNMENT 64

#define WORKER_PRIORITY 2

typedef enum {
  PHASE_IDLE,
  PHASE_WARM_UP,
  PHASE_LOAD,
  PHASE_STOP,
  PHASE_RELEASE
} Phase;

typedef struct {
  void    *ptr;
  uint32_t seq;
  uint32_t size;
  uint32_t expiry;
} Block;

typedef struct {
  Block                items[ RING_SIZE ];
  atomic_uint produced RTEMS_ALIGNED( CACHE_LINE_ALIGNMENT );
  atomic_uint consumed RTEMS_ALIGNED( CACHE_LINE_ALIGNMENT );
} Ring;

typedef struct {
  uint32_t count[ HISTOGRAM_SIZE ];
  uint32_t min;
  uint32_t max;
} Histogram;

typedef struct {
  size_t   free_size;
  size_t   min_free_size;
  uint32_t free_blocks;
  uint32_t used_blocks;
  uint32_t allocs;
  uint32_t failed_allocs;
  uint32_t frees;
} PeriodStatistics;

typedef struct {
  size_t free_size;
  size_t largest_free;
} HeapSample;

typedef struct {
  rtems_id id         RTEMS_ALIGNED( CACHE_LINE_ALIGNMENT );
  uint32_t            index;
  uint32_t            random;
  Block              *short_table;
  uint32_t            short_count;
  uint32_t            short_size;
  Block              *long_table;
  uint32_t            long_count;
  uint32_t            long_size;
  uint32_t            period_index;
  uint32_t            seq;
  uint64_t            sine_step;
  uint32_t            sine_offset;
  uint64_t            time;
  rtems_counter_ticks last;
  uint64_t            period_end;
  uint32_t            period_allocs;
  uint32_t            period_frees;
  bool                load;
  bool                skip_period;
  uint32_t            pattern_errors;
  uint32_t            alignment_errors;
  uint32_t            progress_errors;
  uint32_t            allocs;
  uint32_t            failed_allocs;
  uint32_t            fragmentation_failures;
  uint32_t            frees;
  uint32_t            handovers;
  size_t              held;
  size_t              long_held;
  size_t              peak_held;
  Histogram           malloc_time;
  Histogram           free_time;
  Histogram           failed_free_size;
  Ring                inbox;
} Worker;

/**
 * @brief Test context for spec:/rtems/malloc/val/stress-heap-smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the context of each worker.
   */
  Worker workers[ WORKER_MAX ];

  /**
   * @brief This member contains the sums of the figures of all workers.
   */
  Worker all;

  /**
   * @brief This member contains the count of workers.
   */
  uint32_t worker_count;

  /**
   * @brief This member contains the phase of the run.
   */
  atomic_uint phase;

  /**
   * @brief This member contains the count of acknowledgements of the workers.
   */
  atomic_uint acks;

  /**
   * @brief This member contains the counter value at the start of the run.
   */
  rtems_counter_ticks start;

  /**
   * @brief This member contains the frequency of the CPU counter in Hz.
   */
  uint32_t counter_frequency;

  /**
   * @brief This member contains the length of a period in counter ticks.
   */
  uint64_t period_ticks;

  /**
   * @brief This member contains the sine over one period, as a fraction of
   *   SINE_ONE.
   */
  int16_t sine[ SINE_SIZE ];

  /**
   * @brief This member contains the share of the heap of each worker in bytes.
   */
  size_t share;

  /**
   * @brief This member contains the step of the size sine for the time of a
   *   worker.
   */
  uint64_t size_step;

  /**
   * @brief This member is not zero, while the runner pauses the workers.
   */
  atomic_uint pause;

  /**
   * @brief This member contains the count of the workers which wait in the
   *   pause.
   */
  atomic_uint pause_acks;

  /**
   * @brief This member contains the statistics of the heap at each pause.
   */
  HeapSample samples[ SAMPLE_MAX ];

  /**
   * @brief This member contains the count of samples.
   */
  uint32_t sample_count;

  /**
   * @brief This member references the greedy allocation which restricts the
   *   heap.
   */
  void *greedy;

  /**
   * @brief This member contains the statistics of the heap before the run.
   */
  rtems_malloc_statistics before;

  /**
   * @brief This member contains the statistics of the heap while the workers
   *   hold their blocks.
   */
  rtems_malloc_statistics stopped;

  /**
   * @brief This member contains the statistics of the heap after the run.
   */
  rtems_malloc_statistics after;

  /**
   * @brief This member contains the size of the largest free block while the
   *   workers hold their blocks.
   */
  size_t largest_free;

  /**
   * @brief This member contains the statistics of the heap at the end of each
   *   period.
   */
  PeriodStatistics periods[ PERIOD_MAX ];

  /**
   * @brief This member contains the count of recorded periods.
   */
  uint32_t period_count;

  /**
   * @brief This member is true, if the heap walk reported no error.
   */
  bool walk_ok;

  /**
   * @brief This member is true, if the JSON output holds an entry in the
   *   current list.
   */
  bool json_has_entry;
} RtemsMallocValStressHeapSmp_Context;

static RtemsMallocValStressHeapSmp_Context
  RtemsMallocValStressHeapSmp_Instance;

typedef RtemsMallocValStressHeapSmp_Context Context;

static uint32_t SimpleRandom( uint32_t v )
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static uint32_t NextRandom( Worker *w )
{
  w->random = SimpleRandom( w->random );

  return w->random >> 16;
}

static uint32_t NextRandom32( Worker *w )
{
  uint32_t high;

  high = NextRandom( w );

  return ( high << 16 ) | NextRandom( w );
}

/* The Taylor series reaches an error below 1e-8 in the range of -pi to pi. */
static double Sine( double x )
{
  double term;
  double sum;
  int    k;

  term = x;
  sum = x;

  for ( k = 1; k < 12; ++k ) {
    term *= -x * x / ( ( 2 * k ) * ( 2 * k + 1 ) );
    sum += term;
  }

  return sum;
}

static void InitializeSine( Context *ctx )
{
  uint32_t k;

  for ( k = 0; k < SINE_SIZE; ++k ) {
    double x;

    x = 2.0 * PI * k / SINE_SIZE;

    if ( x > PI ) {
      x -= 2.0 * PI;
    }

    ctx->sine[ k ] = (int16_t) ( SINE_ONE * Sine( x ) );
  }
}

static int32_t SineAt(
  const Context *ctx,
  uint64_t       step,
  uint32_t       offset,
  uint64_t       time
)
{
  uint32_t index;

  index = (uint32_t) ( ( time * step ) >> 32 ) + offset;

  return ctx->sine[ index % SINE_SIZE ];
}

/*
 * The target of a worker is 0.85 plus 0.25 of the sine of its share of the
 * heap.  The sum of the targets of all workers thus lies between 0.6 and 1.1
 * times the heap size.
 */
static size_t Target( const Context *ctx, const Worker *w )
{
  int32_t factor;

  factor = TARGET_BASE +
           ( TARGET_AMPLITUDE *
             SineAt( ctx, w->sine_step, w->sine_offset, w->time ) ) /
             SINE_ONE;

  return (size_t) ( ( (uint64_t) ctx->share * (uint32_t) factor ) >> 16 );
}

static void HistogramInitialize( Histogram *h )
{
  memset( h, 0, sizeof( *h ) );
  h->min = UINT32_MAX;
}

static void HistogramAdd( Histogram *h, uint32_t value )
{
  uint32_t bucket;

  bucket = value == 0 ? 0 : 32 - (uint32_t) __builtin_clz( value );

  if ( bucket >= HISTOGRAM_SIZE ) {
    bucket = HISTOGRAM_SIZE - 1;
  }

  ++h->count[ bucket ];

  if ( value < h->min ) {
    h->min = value;
  }

  if ( value > h->max ) {
    h->max = value;
  }
}

static void HistogramMerge( Histogram *to, const Histogram *from )
{
  uint32_t i;

  for ( i = 0; i < HISTOGRAM_SIZE; ++i ) {
    to->count[ i ] += from->count[ i ];
  }

  if ( from->min < to->min ) {
    to->min = from->min;
  }

  if ( from->max > to->max ) {
    to->max = from->max;
  }
}

/*
 * The median is the lower bound of the bucket which holds it, limited to the
 * range of the values.
 */
static uint32_t HistogramMedian( const Histogram *h )
{
  uint64_t total;
  uint64_t sum;
  uint32_t i;

  total = 0;

  for ( i = 0; i < HISTOGRAM_SIZE; ++i ) {
    total += h->count[ i ];
  }

  sum = 0;

  for ( i = 0; i < HISTOGRAM_SIZE; ++i ) {
    sum += h->count[ i ];

    if ( 2 * sum >= total && total != 0 ) {
      uint32_t median;

      median = i == 0 ? 0 : UINT32_C( 1 ) << ( i - 1 );

      if ( median < h->min ) {
        median = h->min;
      }

      if ( median > h->max ) {
        median = h->max;
      }

      return median;
    }
  }

  return 0;
}

static rtems_counter_ticks UpdateTime( Worker *w )
{
  rtems_counter_ticks now;

  now = rtems_counter_read();
  w->time += rtems_counter_difference( now, w->last );
  w->last = now;

  return now;
}

static uint32_t PatternSeed( uint32_t owner, uint32_t seq, uint32_t size )
{
  return ( ( owner + 1 ) * UINT32_C( 0x9e3779b9 ) ) ^
         ( seq * UINT32_C( 0x85ebca6b ) ) ^ ( size * UINT32_C( 0xc2b2ae35 ) );
}

static void Fill( void *ptr, uint32_t seed, uint32_t size )
{
  uint32_t *words;
  uint8_t  *bytes;
  uint32_t  n;
  uint32_t  k;

  words = ptr;
  n = size / 4;

  for ( k = 0; k < n; ++k ) {
    words[ k ] = seed ^ ( k * UINT32_C( 0x01000193 ) );
  }

  bytes = (uint8_t *) &words[ n ];

  for ( k = 0; k < size % 4; ++k ) {
    bytes[ k ] = (uint8_t) ( seed >> ( 8 * k ) );
  }
}

static bool IsFilled( const void *ptr, uint32_t seed, uint32_t size )
{
  const uint32_t *words;
  const uint8_t  *bytes;
  uint32_t        n;
  uint32_t        k;

  words = ptr;
  n = size / 4;

  for ( k = 0; k < n; ++k ) {
    if ( words[ k ] != ( seed ^ ( k * UINT32_C( 0x01000193 ) ) ) ) {
      return false;
    }
  }

  bytes = (const uint8_t *) &words[ n ];

  for ( k = 0; k < size % 4; ++k ) {
    if ( bytes[ k ] != (uint8_t) ( seed >> ( 8 * k ) ) ) {
      return false;
    }
  }

  return true;
}

/*
 * The probability of a large block follows the size sine shared by all
 * workers.  The size is uniform within an octave and the octave is uniform.
 */
static uint32_t DrawSize( const Context *ctx, Worker *w )
{
  int32_t  s;
  uint32_t e;
  uint32_t base;

  s = SineAt( ctx, ctx->size_step, 0, w->time );

  if ( NextRandom( w ) < (uint32_t) ( s + SINE_ONE + 1 ) ) {
    e = LARGE_OCTAVE_MIN + NextRandom( w ) % LARGE_OCTAVE_COUNT;
  } else {
    e = SMALL_OCTAVE_MIN + NextRandom( w ) % SMALL_OCTAVE_COUNT;
  }

  base = UINT32_C( 1 ) << e;

  return base + NextRandom( w ) % base;
}

static bool Push( Ring *ring, const Block *block )
{
  unsigned int produced;
  unsigned int consumed;

  produced = atomic_load_explicit( &ring->produced, memory_order_relaxed );
  consumed = atomic_load_explicit( &ring->consumed, memory_order_acquire );

  if ( produced - consumed >= RING_SIZE ) {
    return false;
  }

  ring->items[ produced % RING_SIZE ] = *block;
  atomic_store_explicit( &ring->produced, produced + 1, memory_order_release );

  return true;
}

static void FreeBlock( Worker *w, uint32_t owner, const Block *block )
{
  rtems_counter_ticks t0;
  rtems_counter_ticks t1;

  if ( !IsFilled(
         block->ptr,
         PatternSeed( owner, block->seq, block->size ),
         block->size
       ) ) {
    ++w->pattern_errors;
  }

  t0 = UpdateTime( w );
  free( block->ptr );
  t1 = UpdateTime( w );

  ++w->period_frees;

  if ( w->load ) {
    ++w->frees;
    HistogramAdd( &w->free_time, rtems_counter_difference( t1, t0 ) );
  }
}

static void Drain( Worker *w, uint32_t owner )
{
  Ring        *ring;
  unsigned int produced;
  unsigned int consumed;

  ring = &w->inbox;
  consumed = atomic_load_explicit( &ring->consumed, memory_order_relaxed );
  produced = atomic_load_explicit( &ring->produced, memory_order_acquire );

  while ( consumed != produced ) {
    Block block;

    block = ring->items[ consumed % RING_SIZE ];
    ++consumed;
    atomic_store_explicit( &ring->consumed, consumed, memory_order_release );
    FreeBlock( w, owner, &block );
  }
}

static void Release( Context *ctx, Worker *w );

static void Allocate( Context *ctx, Worker *w )
{
  rtems_counter_ticks t0;
  rtems_counter_ticks t1;
  uint32_t            size;
  uint32_t            r;
  size_t              alignment;
  void               *ptr;
  Block              *block;

  size = DrawSize( ctx, w );
  r = NextRandom( w );

  if ( ( r & 7 ) == 0 ) {
    int eno;

    alignment = (size_t) 8 << ( ( r >> 3 ) % 10 );
    t0 = UpdateTime( w );
    eno = posix_memalign( &ptr, alignment, size );
    t1 = UpdateTime( w );

    if ( eno != 0 ) {
      ptr = NULL;
    }
  } else {
    alignment = 8;
    t0 = UpdateTime( w );
    ptr = malloc( size );
    t1 = UpdateTime( w );
  }

  ++w->period_allocs;

  if ( w->load ) {
    HistogramAdd( &w->malloc_time, rtems_counter_difference( t1, t0 ) );
  }

  if ( ptr == NULL ) {
    if ( w->load ) {
      rtems_malloc_statistics stats;

      ++w->failed_allocs;
      (void) malloc_get_statistics( &stats );
      HistogramAdd( &w->failed_free_size, (uint32_t) stats.free_size );

      if ( stats.free_size >= size + alignment ) {
        ++w->fragmentation_failures;
      }
    }

    if ( w->short_count > 0 ) {
      Release( ctx, w );
    }

    return;
  }

  if ( (uintptr_t) ptr % alignment != 0 ) {
    ++w->alignment_errors;
  }

  ++w->seq;
  Fill( ptr, PatternSeed( w->index, w->seq, size ), size );

  if (
    ( NextRandom( w ) & 7 ) == 0 && w->long_count < w->long_size &&
    w->long_held + size <= ctx->share / 2
  ) {
    block = &w->long_table[ w->long_count ];
    ++w->long_count;
    w->long_held += size;
    block->expiry = w->period_index + LONG_PERIODS_MIN +
                    NextRandom( w ) % LONG_PERIODS_SPAN;
  } else {
    block = &w->short_table[ w->short_count ];
    ++w->short_count;
    block->expiry = 0;
  }

  block->ptr = ptr;
  block->seq = w->seq;
  block->size = size;
  w->held += size;

  if ( w->held > w->peak_held ) {
    w->peak_held = w->held;
  }

  if ( w->load ) {
    ++w->allocs;
  }
}

static void Release( Context *ctx, Worker *w )
{
  uint32_t i;
  Block    block;

  i = NextRandom32( w ) % w->short_count;
  block = w->short_table[ i ];
  --w->short_count;
  w->short_table[ i ] = w->short_table[ w->short_count ];
  w->held -= block.size;

  if (
    ctx->worker_count > 1 && ( NextRandom( w ) & 7 ) == 0 &&
    Push( &ctx->workers[ ( w->index + 1 ) % ctx->worker_count ].inbox, &block )
  ) {
    if ( w->load ) {
      ++w->handovers;
    }

    return;
  }

  FreeBlock( w, w->index, &block );
}

static void FreeExpired( Worker *w )
{
  uint32_t i;

  i = 0;

  while ( i < w->long_count ) {
    Block *block;

    block = &w->long_table[ i ];

    if ( (int32_t) ( w->period_index - block->expiry ) >= 0 ) {
      Block expired;

      expired = *block;
      --w->long_count;
      *block = w->long_table[ w->long_count ];
      w->held -= expired.size;
      w->long_held -= expired.size;
      FreeBlock( w, w->index, &expired );
    } else {
      ++i;
    }
  }
}

static void StartLoad( Worker *w )
{
  w->load = true;
  w->skip_period = true;
  w->allocs = 0;
  w->failed_allocs = 0;
  w->fragmentation_failures = 0;
  w->frees = 0;
  w->handovers = 0;
  w->peak_held = w->held;
  HistogramInitialize( &w->malloc_time );
  HistogramInitialize( &w->free_time );
  HistogramInitialize( &w->failed_free_size );
}

static void RecordPeriod( Context *ctx )
{
  rtems_malloc_statistics stats;
  PeriodStatistics       *period;

  if ( ctx->period_count >= PERIOD_MAX ) {
    return;
  }

  (void) malloc_get_statistics( &stats );
  period = &ctx->periods[ ctx->period_count ];
  ++ctx->period_count;
  period->free_size = stats.free_size;
  period->min_free_size = stats.min_free_size;
  period->free_blocks = stats.free_blocks;
  period->used_blocks = stats.used_blocks;
  period->allocs = stats.allocs;
  period->failed_allocs = stats.failed_allocs;
  period->frees = stats.frees;
}

static void EndPeriods( Context *ctx, Worker *w )
{
  while ( w->time >= w->period_end ) {
    if ( w->load ) {
      if ( w->skip_period ) {
        w->skip_period = false;
      } else if ( w->period_allocs == 0 || w->period_frees == 0 ) {
        ++w->progress_errors;
      }

      if ( w->index == 0 ) {
        RecordPeriod( ctx );
      }
    }

    w->period_allocs = 0;
    w->period_frees = 0;
    w->period_end += ctx->period_ticks;
    ++w->period_index;
    FreeExpired( w );
  }
}

static unsigned int GetPhase( const Context *ctx )
{
  return atomic_load_explicit( &ctx->phase, memory_order_acquire );
}

static void WaitForPhase( const Context *ctx, unsigned int phase )
{
  while ( GetPhase( ctx ) != phase ) {
    /* Wait */
  }
}

static void Acknowledge( Context *ctx )
{
  (void) atomic_fetch_add_explicit( &ctx->acks, 1, memory_order_release );
}

static void WaitWhilePaused( Context *ctx )
{
  if ( atomic_load_explicit( &ctx->pause, memory_order_acquire ) == 0 ) {
    return;
  }

  (void)
    atomic_fetch_add_explicit( &ctx->pause_acks, 1, memory_order_release );

  while ( atomic_load_explicit( &ctx->pause, memory_order_acquire ) != 0 ) {
    /* Wait */
  }
}

static void WorkerBody( rtems_task_argument arg )
{
  Context *ctx;
  Worker  *w;
  uint32_t owner;

  ctx = T_fixture_context();
  w = &ctx->workers[ arg ];
  owner = ( w->index + ctx->worker_count - 1 ) % ctx->worker_count;
  w->last = ctx->start;
  w->time = 0;
  w->period_end = ctx->period_ticks;

  while ( GetPhase( ctx ) <= PHASE_LOAD ) {
    WaitWhilePaused( ctx );

    if ( GetPhase( ctx ) == PHASE_LOAD && !w->load ) {
      StartLoad( w );
    }

    Drain( w, owner );
    (void) UpdateTime( w );
    EndPeriods( ctx, w );

    if ( w->short_count == w->short_size ) {
      Release( ctx, w );
    } else if ( w->held < Target( ctx, w ) ) {
      Allocate( ctx, w );
    } else if ( w->short_count > 0 ) {
      Release( ctx, w );
    }
  }

  Acknowledge( ctx );
  WaitForPhase( ctx, PHASE_RELEASE );
  Drain( w, owner );

  while ( w->short_count > 0 ) {
    --w->short_count;
    FreeBlock( w, w->index, &w->short_table[ w->short_count ] );
  }

  while ( w->long_count > 0 ) {
    --w->long_count;
    FreeBlock( w, w->index, &w->long_table[ w->long_count ] );
  }

  w->held = 0;
  w->long_held = 0;
  Acknowledge( ctx );
  (void) rtems_task_suspend( RTEMS_SELF );
}

static void WaitForAcknowledgements( Context *ctx, unsigned int count )
{
  while ( atomic_load_explicit( &ctx->acks, memory_order_acquire ) != count ) {
    (void) rtems_task_wake_after( 1 );
  }
}

static void SetPhase( Context *ctx, unsigned int phase )
{
  atomic_store_explicit( &ctx->phase, phase, memory_order_release );
}

static void Sleep( uint32_t periods )
{
  rtems_status_code sc;
  rtems_interval    ticks;

  ticks = rtems_clock_get_ticks_per_second() * periods / PERIODS_PER_SECOND;
  sc = rtems_task_wake_after( ticks );
  T_quiet_rsc_success( sc );
}

static uint32_t CountAlignmentErrors( const Context *ctx )
{
  uint32_t count;
  uint32_t i;

  count = 0;

  for ( i = 0; i < ctx->worker_count; ++i ) {
    count += ctx->workers[ i ].alignment_errors;
  }

  return count;
}

/*
 * A worker without an allocation or without a free in the load time counts
 * as one progress error.
 */
static uint32_t CountProgressErrors( const Context *ctx )
{
  uint32_t count;
  uint32_t i;

  count = 0;

  for ( i = 0; i < ctx->worker_count; ++i ) {
    const Worker *w;

    w = &ctx->workers[ i ];
    count += w->progress_errors;

    if ( w->allocs == 0 || w->frees == 0 ) {
      ++count;
    }
  }

  return count;
}

static uint32_t CountPatternErrors( const Context *ctx )
{
  uint32_t count;
  uint32_t i;

  count = 0;

  for ( i = 0; i < ctx->worker_count; ++i ) {
    count += ctx->workers[ i ].pattern_errors;
  }

  return count;
}

static void Sample( Context *ctx )
{
  rtems_malloc_statistics stats;
  HeapSample             *sample;

  atomic_store_explicit( &ctx->pause_acks, 0, memory_order_relaxed );
  atomic_store_explicit( &ctx->pause, 1, memory_order_release );

  while (
    atomic_load_explicit( &ctx->pause_acks, memory_order_acquire ) !=
    ctx->worker_count
  ) {
    (void) rtems_task_wake_after( 1 );
  }

  (void) malloc_get_statistics( &stats );

  if ( ctx->sample_count < SAMPLE_MAX ) {
    sample = &ctx->samples[ ctx->sample_count ];
    ++ctx->sample_count;
    sample->free_size = stats.free_size;
    sample->largest_free = malloc_free_space();
  }

  atomic_store_explicit( &ctx->pause, 0, memory_order_release );
}

static uint64_t TicksToNanoseconds( const Context *ctx, uint32_t ticks )
{
  return ( (uint64_t) ticks * 1000000000 ) / ctx->counter_frequency;
}

static void PrintSeparator( Context *ctx )
{
  printk( "%s", ctx->json_has_entry ? ",\n" : "" );
  ctx->json_has_entry = true;
}

static void PrintTime(
  Context         *ctx,
  const char      *op,
  const char      *variant,
  const Histogram *h
)
{
  static const char *const parts[] = { "min", "median", "max" };
  uint32_t                 values[ 3 ];
  size_t                   i;

  values[ 0 ] = h->min == UINT32_MAX ? 0 : h->min;
  values[ 1 ] = HistogramMedian( h );
  values[ 2 ] = h->max;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( parts ); ++i ) {
    PrintSeparator( ctx );
    printk(
      "{\"name\": \"stress-heap-smp/%s %s\", \"variant\": \"%s\", "
      "\"samples\": [%" PRIu64 "e-9]}",
      op,
      parts[ i ],
      variant,
      TicksToNanoseconds( ctx, values[ i ] )
    );
  }
}

static void PrintMetric(
  Context    *ctx,
  const char *name,
  const char *variant,
  const char *unit,
  uint64_t    value
)
{
  PrintSeparator( ctx );
  printk(
    "{\"name\": \"stress-heap-smp/%s\", \"variant\": \"%s\", "
    "\"unit\": \"%s\", \"values\": [%" PRIu64 "]}",
    name,
    variant,
    unit,
    value
  );
}

static void PrintHistogram(
  Context         *ctx,
  const char      *name,
  const char      *variant,
  const Histogram *h
)
{
  uint32_t i;

  PrintSeparator( ctx );
  printk(
    "{\"name\": \"stress-heap-smp/%s log2 histogram\", \"variant\": \"%s\", "
    "\"unit\": \"count\", \"values\": [",
    name,
    variant
  );

  for ( i = 0; i < HISTOGRAM_SIZE; ++i ) {
    printk( "%s%" PRIu32, i == 0 ? "" : ", ", h->count[ i ] );
  }

  printk( "]}" );
}

#define PRINT_PERIODS( ctx, member, unit )                        \
  do {                                                            \
    uint32_t i_;                                                  \
    PrintSeparator( ctx );                                        \
    printk(                                                       \
      "{\"name\": \"stress-heap-smp/period " #member "\", "       \
      "\"variant\": \"0\", \"unit\": \"" unit "\", \"values\": [" \
    );                                                            \
    for ( i_ = 0; i_ < ( ctx )->period_count; ++i_ ) {            \
      printk(                                                     \
        "%s%" PRIu64,                                             \
        i_ == 0 ? "" : ", ",                                      \
        (uint64_t) ( ctx )->periods[ i_ ].member                  \
      );                                                          \
    }                                                             \
    printk( "]}" );                                               \
  } while ( 0 )

static void PrintWorker( Context *ctx, const char *variant, const Worker *w )
{
  PrintTime( ctx, "malloc", variant, &w->malloc_time );
  PrintTime( ctx, "free", variant, &w->free_time );
}

static void PrintCounts( Context *ctx, const char *variant, const Worker *w )
{
  PrintMetric( ctx, "allocs", variant, "count", w->allocs );
  PrintMetric( ctx, "failed allocs", variant, "count", w->failed_allocs );
  PrintMetric(
    ctx,
    "fragmentation failures",
    variant,
    "count",
    w->fragmentation_failures
  );
  PrintMetric( ctx, "frees", variant, "count", w->frees );
  PrintMetric( ctx, "handovers", variant, "count", w->handovers );
  PrintMetric( ctx, "peak held", variant, "B", w->peak_held );
  PrintMetric( ctx, "pattern errors", variant, "count", w->pattern_errors );
  PrintMetric(
    ctx,
    "alignment errors",
    variant,
    "count",
    w->alignment_errors
  );
  PrintMetric( ctx, "progress errors", variant, "count", w->progress_errors );
  PrintHistogram( ctx, "malloc ticks", variant, &w->malloc_time );
  PrintHistogram( ctx, "free ticks", variant, &w->free_time );
  PrintHistogram(
    ctx,
    "failed alloc free size",
    variant,
    &w->failed_free_size
  );
}

static void SumWorkers( Context *ctx, Worker *all )
{
  uint32_t i;

  memset( all, 0, sizeof( *all ) );
  HistogramInitialize( &all->malloc_time );
  HistogramInitialize( &all->free_time );
  HistogramInitialize( &all->failed_free_size );

  for ( i = 0; i < ctx->worker_count; ++i ) {
    const Worker *w;

    w = &ctx->workers[ i ];
    all->allocs += w->allocs;
    all->failed_allocs += w->failed_allocs;
    all->fragmentation_failures += w->fragmentation_failures;
    all->frees += w->frees;
    all->handovers += w->handovers;
    all->peak_held += w->peak_held;
    all->pattern_errors += w->pattern_errors;
    all->alignment_errors += w->alignment_errors;
    all->progress_errors += w->progress_errors;
    HistogramMerge( &all->malloc_time, &w->malloc_time );
    HistogramMerge( &all->free_time, &w->free_time );
    HistogramMerge( &all->failed_free_size, &w->failed_free_size );
  }
}

static uint64_t Fragmentation( size_t largest_free, size_t free_size )
{
  if ( free_size == 0 ) {
    return 0;
  }

  return 1000000 - ( (uint64_t) largest_free * 1000000 ) / free_size;
}

static void PrintSamples( Context *ctx )
{
  uint32_t i;

  PrintSeparator( ctx );
  printk(
    "{\"name\": \"stress-heap-smp/sample fragmentation\", \"variant\": \"all\", "
    "\"unit\": \"ppm\", \"values\": ["
  );

  for ( i = 0; i < ctx->sample_count; ++i ) {
    printk(
      "%s%" PRIu64,
      i == 0 ? "" : ", ",
      Fragmentation(
        ctx->samples[ i ].largest_free,
        ctx->samples[ i ].free_size
      )
    );
  }

  printk( "]}" );
  PrintSeparator( ctx );
  printk(
    "{\"name\": \"stress-heap-smp/sample free size\", \"variant\": \"all\", "
    "\"unit\": \"B\", \"values\": ["
  );

  for ( i = 0; i < ctx->sample_count; ++i ) {
    printk(
      "%s%" PRIu64,
      i == 0 ? "" : ", ",
      (uint64_t) ctx->samples[ i ].free_size
    );
  }

  printk( "]}" );
  PrintSeparator( ctx );
  printk(
    "{\"name\": \"stress-heap-smp/sample largest free block\", "
    "\"variant\": \"all\", \"unit\": \"B\", \"values\": ["
  );

  for ( i = 0; i < ctx->sample_count; ++i ) {
    printk(
      "%s%" PRIu64,
      i == 0 ? "" : ", ",
      (uint64_t) ctx->samples[ i ].largest_free
    );
  }

  printk( "]}" );
}

static void PrintFigures( Context *ctx )
{
  char     variant[ 16 ];
  uint32_t i;

  SumWorkers( ctx, &ctx->all );

  printk(
    "*** BEGIN OF JSON ***\n"
    "{\"system\": \"%s\", \"processors\": %" PRIu32 ", "
    "\"heap-size\": %" PRIu64 ", \"counter-frequency\": %" PRIu32 ", "
    "\"period-ticks\": %" PRIu64 ", \"warm-up-periods\": %i, "
    "\"load-seconds\": %i, \"measurements\": [\n",
    rtems_get_version_string(),
    ctx->worker_count,
    (uint64_t) ctx->before.free_size,
    ctx->counter_frequency,
    ctx->period_ticks,
    WARM_UP_PERIODS,
    LOAD_SECONDS
  );

  ctx->json_has_entry = false;

  for ( i = 0; i < ctx->worker_count; ++i ) {
    (void) T_snprintf( variant, sizeof( variant ), "%" PRIu32, i );
    PrintWorker( ctx, variant, &ctx->workers[ i ] );
  }

  PrintWorker( ctx, "all", &ctx->all );
  printk( "\n], \"metrics\": [\n" );
  ctx->json_has_entry = false;

  for ( i = 0; i < ctx->worker_count; ++i ) {
    (void) T_snprintf( variant, sizeof( variant ), "%" PRIu32, i );
    PrintCounts( ctx, variant, &ctx->workers[ i ] );
  }

  PrintCounts( ctx, "all", &ctx->all );
  PrintMetric( ctx, "largest free block", "all", "B", ctx->largest_free );
  PrintMetric( ctx, "free size", "all", "B", ctx->stopped.free_size );
  PrintMetric(
    ctx,
    "fragmentation",
    "all",
    "ppm",
    Fragmentation( ctx->largest_free, ctx->stopped.free_size )
  );
  PrintSamples( ctx );
  PRINT_PERIODS( ctx, free_size, "B" );
  PRINT_PERIODS( ctx, min_free_size, "B" );
  PRINT_PERIODS( ctx, free_blocks, "count" );
  PRINT_PERIODS( ctx, used_blocks, "count" );
  PRINT_PERIODS( ctx, allocs, "count" );
  PRINT_PERIODS( ctx, failed_allocs, "count" );
  PRINT_PERIODS( ctx, frees, "count" );
  printk( "\n]}\n*** END OF JSON ***\n" );
}

static void RtemsMallocValStressHeapSmp_Setup(
  RtemsMallocValStressHeapSmp_Context *ctx
)
{
  rtems_status_code sc;
  uintptr_t         heap_size;
  uint32_t          short_size;
  uint32_t          n;
  uint32_t          i;
  int               rv;

  n = rtems_scheduler_get_processor_maximum();
  T_assert_le_u32( n, WORKER_MAX );
  ctx->worker_count = n;
  ctx->counter_frequency = rtems_counter_frequency();
  ctx->period_ticks = ctx->counter_frequency / PERIODS_PER_SECOND;
  InitializeSine( ctx );
  ctx->share = HEAP_SIZE / n;
  ctx->size_step = ( (uint64_t) SINE_SIZE << 32 ) /
                   ( ctx->period_ticks * SIZE_PERIODS );
  short_size = (uint32_t) ( 2 * ctx->share / SIZE_MEAN_MIN );

  for ( i = 0; i < n; ++i ) {
    Worker   *w;
    cpu_set_t set;

    w = &ctx->workers[ i ];
    w->index = i;
    w->random = UINT32_C( 0x12345678 ) + i * UINT32_C( 0x9e3779b9 );
    w->short_size = short_size;
    w->short_table = malloc( short_size * sizeof( *w->short_table ) );
    T_assert_not_null( w->short_table );
    w->long_size = short_size / 4;
    w->long_table = malloc( w->long_size * sizeof( *w->long_table ) );
    T_assert_not_null( w->long_table );
    w->sine_step = ( ( (uint64_t) SINE_SIZE * ( n + i ) ) << 32 ) /
                   ( ctx->period_ticks * n );
    w->sine_offset = SINE_SIZE * i / n;
    HistogramInitialize( &w->malloc_time );
    HistogramInitialize( &w->free_time );
    HistogramInitialize( &w->failed_free_size );

    sc = rtems_task_create(
      rtems_build_name( 'W', 'O', 'R', 'K' ),
      WORKER_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &w->id
    );
    T_assert_rsc_success( sc );

    CPU_ZERO( &set );
    CPU_SET( (int) i, &set );
    sc = rtems_task_set_affinity( w->id, sizeof( set ), &set );
    T_assert_rsc_success( sc );
  }

  heap_size = HEAP_SIZE;
  ctx->greedy = rtems_heap_greedy_allocate( &heap_size, 1 );
  rv = malloc_get_statistics( &ctx->before );
  T_assert_eq_int( rv, 0 );
  T_assert_ge_sz( ctx->before.free_size, HEAP_SIZE );
}

static void RtemsMallocValStressHeapSmp_Setup_Wrap( void *arg )
{
  RtemsMallocValStressHeapSmp_Context *ctx;

  ctx = arg;
  RtemsMallocValStressHeapSmp_Setup( ctx );
}

static void RtemsMallocValStressHeapSmp_Teardown(
  RtemsMallocValStressHeapSmp_Context *ctx
)
{
  uint32_t i;

  for ( i = 0; i < ctx->worker_count; ++i ) {
    Worker *w;

    w = &ctx->workers[ i ];

    if ( w->id != 0 ) {
      (void) rtems_task_delete( w->id );
    }

    free( w->short_table );
    free( w->long_table );
  }

  if ( ctx->greedy != NULL ) {
    rtems_heap_greedy_free( ctx->greedy );
  }
}

static void RtemsMallocValStressHeapSmp_Teardown_Wrap( void *arg )
{
  RtemsMallocValStressHeapSmp_Context *ctx;

  ctx = arg;
  RtemsMallocValStressHeapSmp_Teardown( ctx );
}

static T_fixture RtemsMallocValStressHeapSmp_Fixture = {
  .setup = RtemsMallocValStressHeapSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMallocValStressHeapSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsMallocValStressHeapSmp_Instance
};

/**
 * @brief Start one worker on each processor. Let the workers allocate and free
 *   blocks for the warm-up periods and for the load time. Pause the workers
 *   once per second to sample the heap. Stop the workers while they hold their
 *   blocks.
 */
static void RtemsMallocValStressHeapSmp_Action_0(
  RtemsMallocValStressHeapSmp_Context *ctx
)
{
  rtems_status_code sc;
  uint32_t          i;

  ctx->start = rtems_counter_read();
  SetPhase( ctx, PHASE_WARM_UP );

  for ( i = 0; i < ctx->worker_count; ++i ) {
    sc = rtems_task_start( ctx->workers[ i ].id, WorkerBody, i );
    T_assert_rsc_success( sc );
  }

  Sleep( WARM_UP_PERIODS );
  SetPhase( ctx, PHASE_LOAD );

  for ( i = 0; i < LOAD_SECONDS; ++i ) {
    Sleep( PERIODS_PER_SECOND );
    Sample( ctx );
  }

  SetPhase( ctx, PHASE_STOP );
  WaitForAcknowledgements( ctx, ctx->worker_count );

  /*
   * Check that each block which a worker allocated meets the requested
   * alignment.
   */
  T_eq_u32( CountAlignmentErrors( ctx ), 0 );

  /*
   * Check that each worker completed at least one allocation and one free in
   * each period of the load time.
   */
  T_eq_u32( CountProgressErrors( ctx ), 0 );
}

/**
 * @brief Get the statistics of the heap and the size of the largest free block
 *   while the workers hold their blocks.
 */
static void RtemsMallocValStressHeapSmp_Action_1(
  RtemsMallocValStressHeapSmp_Context *ctx
)
{
  int rv;

  rv = malloc_get_statistics( &ctx->stopped );
  T_assert_eq_int( rv, 0 );
  ctx->largest_free = malloc_free_space();

  /*
   * Check that the largest free block is not larger than the free size.
   */
  T_le_sz( ctx->largest_free, ctx->stopped.free_size );
}

/**
 * @brief Let the workers free all their blocks. Walk the heap. Get the
 *   statistics of the heap.
 */
static void RtemsMallocValStressHeapSmp_Action_2(
  RtemsMallocValStressHeapSmp_Context *ctx
)
{
  int rv;

  SetPhase( ctx, PHASE_RELEASE );
  WaitForAcknowledgements( ctx, 2 * ctx->worker_count );
  ctx->walk_ok = malloc_walk( 0, false );
  rv = malloc_get_statistics( &ctx->after );
  T_assert_eq_int( rv, 0 );

  /*
   * Check that each block carried the fill pattern of its owner, its sequence
   * number and its size when it was freed.
   */
  T_eq_u32( CountPatternErrors( ctx ), 0 );

  /*
   * Check that the heap walk reported no error.
   */
  T_true( ctx->walk_ok );

  /*
   * Check that the free size after the run is equal to the free size before
   * the run.
   */
  T_eq_sz( ctx->after.free_size, ctx->before.free_size );
}

/**
 * @brief Print the figures of the workers and of the heap.
 */
static void RtemsMallocValStressHeapSmp_Action_3(
  RtemsMallocValStressHeapSmp_Context *ctx
)
{
  PrintFigures( ctx );
}

/**
 * @fn void T_case_body_RtemsMallocValStressHeapSmp( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMallocValStressHeapSmp,
  &RtemsMallocValStressHeapSmp_Fixture
)
{
  RtemsMallocValStressHeapSmp_Context *ctx;

  ctx = T_fixture_context();

  RtemsMallocValStressHeapSmp_Action_0( ctx );
  RtemsMallocValStressHeapSmp_Action_1( ctx );
  RtemsMallocValStressHeapSmp_Action_2( ctx );
  RtemsMallocValStressHeapSmp_Action_3( ctx );
}

/** @} */
