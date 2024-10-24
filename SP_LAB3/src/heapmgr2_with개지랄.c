/*--------------------------------------------------------------------*/
/* heapmgr2.c                                                      */
/* Author: Lee, Ha Dong                                */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"

#define FALSE 0
#define TRUE 1
#define MIN_EXPONENTIAL_BLOCK_UNITS 1
#define MAX_EXPONENTIAL_BLOCK_UNITS 32

#define MIN_FIXED_BLOCK_UNITS 33
#define FIXED_BLOCK_INTERVAL 32
#define MAX_FIXED_BLOCK_UNITS 65536

#define EXPONENTIAL_BINS 6
#define FIXED_BINS 2047

enum {
  MEMALLOC_MIN = 1024,
};

// function declaration
static Chunk_T *find_bin(size_t units);
static Chunk_T *find_best_fit_bin(size_t units);
static size_t size_to_units(size_t size);
static Chunk_T get_chunk_from_data_ptr(void *m);
static void init_my_heap(void);
static Chunk_T merge_chunk(Chunk_T c1, Chunk_T c2);
static Chunk_T split_and_relocate_remaining_chunk(Chunk_T c, size_t units);
static void insert_chunk(Chunk_T c);
static void remove_chunk_from_list(Chunk_T c);
static Chunk_T allocate_more_memory(size_t units);

/* g_free_head: point to first chunk in the free list */
// static Chunk_T g_free_head = NULL;
/* g_heap_start, g_heap_end: start and end of the heap area.
 * g_heap_end will move if you increase the heap */

// from 1 units to 32 units (16 bytes to 512 bytes)
/*
0th bin: 1 unit ~ 1 unit
1th bin: 2 units ~ 2 units
2th bin: 3 units ~ 4 units
3th bin: 5 units ~ 8 units
4th bin: 9 units ~ 16 units
5th bin: 17 units ~ 32 units
*/
static Chunk_T exponential_bins[EXPONENTIAL_BINS];

// from 33 units to 65536 units (512 bytes to 1MB)
/*
0th bin: 33 units ~ 64 units 2
1th bin: 65 units ~ 96 units
2th bin: 97 units ~ 128 units
...
last bin: 65505 units ~ 65536 units (1MB) 2048
*/
static Chunk_T fixed_bins[FIXED_BINS];

static Chunk_T large_chunk_free_head = NULL;

static void *g_heap_start = NULL, *g_heap_end = NULL;

#ifndef NDEBUG
/* check_heap_validity:
 * Validity check for entire data structures for chunks. Note that this
 * is basic sanity check, and passing this test does not guarantee the
 * integrity of your code.
 * Returns 1 on success or 0 (zero) on failure.
 */
static int check_heap_validity(void) {
  Chunk_T w;

  if (g_heap_start == NULL) {
    fprintf(stderr, "Uninitialized heap start\n");
    return FALSE;
  }

  if (g_heap_end == NULL) {
    fprintf(stderr, "Uninitialized heap end\n");
    return FALSE;
  }

  // 지수 범위의 모든 bin을 순차적으로 검사
  for (int i = 0; i < EXPONENTIAL_BINS; i++) {
    size_t min_units =
        (i == 0) ? 1 : (MIN_EXPONENTIAL_BLOCK_UNITS << (i - 1)) + 1;
    size_t max_units = MIN_EXPONENTIAL_BLOCK_UNITS << i;

    for (w = exponential_bins[i]; w != NULL; w = chunk_get_next_free_chunk(w)) {
      size_t units = chunk_get_units(w);

      // 청크가 올바른 범위의 유닛을 가지는지 검사
      if (units < min_units || units > max_units) {
        fprintf(stderr,
                "Chunk in exponential bin %d has invalid units (%zu not in "
                "[%zu, %zu])\n",
                i, units, min_units, max_units);
        return FALSE;
      }

      if (chunk_get_status(w) != CHUNK_FREE) {
        fprintf(stderr, "Non-free chunk in exponential bin %d\n", i);
        return FALSE;
      }

      if (!chunk_is_valid(w, g_heap_start, g_heap_end)) {
        fprintf(stderr, "Invalid chunk in exponential bin %d\n", i);
        return FALSE;
      }
    }
  }

  // 고정 범위의 모든 bin을 순차적으로 검사
  for (int i = 0; i < FIXED_BINS; i++) {
    size_t min_units = MIN_FIXED_BLOCK_UNITS + (i * FIXED_BLOCK_INTERVAL);
    size_t max_units = min_units + FIXED_BLOCK_INTERVAL - 1;

    for (w = fixed_bins[i]; w != NULL; w = chunk_get_next_free_chunk(w)) {
      size_t units = chunk_get_units(w);

      // 청크가 올바른 범위의 유닛을 가지는지 검사
      if (units < min_units || units > max_units) {
        fprintf(
            stderr,
            "Chunk in fixed bin %d has invalid units (%zu not in [%zu, %zu])\n",
            i, units, min_units, max_units);
        return FALSE;
      }

      if (chunk_get_status(w) != CHUNK_FREE) {
        fprintf(stderr, "Non-free chunk in fixed bin %d\n", i);
        return FALSE;
      }

      if (!chunk_is_valid(w, g_heap_start, g_heap_end)) {
        fprintf(stderr, "Invalid chunk in fixed bin %d\n", i);
        return FALSE;
      }
    }
  }

  // 큰 청크 관리 리스트 검사
  for (w = large_chunk_free_head; w != NULL; w = chunk_get_next_free_chunk(w)) {
    size_t units = chunk_get_units(w);
    if (units <= MAX_FIXED_BLOCK_UNITS) {
      fprintf(stderr,
              "Large chunk has invalid units (%zu should be greater than %u)\n",
              units, MAX_FIXED_BLOCK_UNITS);
      return FALSE;
    }

    if (chunk_get_status(w) != CHUNK_FREE) {
      fprintf(stderr, "Non-free chunk in large chunk list\n");
      return FALSE;
    }

    if (!chunk_is_valid(w, g_heap_start, g_heap_end)) {
      fprintf(stderr, "Invalid chunk in large chunk list\n");
      return FALSE;
    }
  }

  return TRUE;
}
#endif

// todo: 주어진 청크를 삽입하는 경우에, head가 나타내는 범위랑 해당 청크의
// todo: 유닛이 일치하는지 assert하는 로직이 필요함

// 지수 범위 초기화
static void init_exponential_bins() {
  for (int i = 0; i < EXPONENTIAL_BINS; i++) {
    exponential_bins[i] = NULL;
  }
}

// 고정 범위 초기화
static void init_fixed_bins() {
  for (int i = 0; i < FIXED_BINS; i++) {
    fixed_bins[i] = NULL;
  }
}

static Chunk_T *find_bin(size_t units) {
  if (units <= MAX_EXPONENTIAL_BLOCK_UNITS) {
    for (int i = 0; i < EXPONENTIAL_BINS; i++) {
      size_t bin_unit_size = MIN_EXPONENTIAL_BLOCK_UNITS << i;
      if (units <= bin_unit_size) {
        return &exponential_bins[i];
      }
    }
  } else if (units <= MAX_FIXED_BLOCK_UNITS) {
    for (int i = 0; i < FIXED_BINS; i++) {
      // size_t min_units = MIN_FIXED_BLOCK_UNITS + (i * FIXED_BLOCK_INTERVAL);
      // size_t max_units = min_units + FIXED_BLOCK_INTERVAL - 1;

      size_t bin_unit_size =
          MIN_FIXED_BLOCK_UNITS + (i + 1) * FIXED_BLOCK_INTERVAL - 1;
      if (units <= bin_unit_size) {
        return &fixed_bins[i];
      }
    }
  }

  return &large_chunk_free_head;
}

static Chunk_T *find_best_fit_bin(size_t units) {
  if (units <= MAX_EXPONENTIAL_BLOCK_UNITS) {
    for (int i = 0; i < EXPONENTIAL_BINS; i++) {
      size_t min_units =
          (i == 0) ? 1 : (MIN_EXPONENTIAL_BLOCK_UNITS << (i - 1)) + 1;
      size_t max_units = MIN_EXPONENTIAL_BLOCK_UNITS << i;

      size_t bin_unit_size = MIN_EXPONENTIAL_BLOCK_UNITS << i;
      if (units <= bin_unit_size && exponential_bins[i] != NULL) {
        printf("return exp address: %p", (void *)&exponential_bins[i]);
        return &exponential_bins[i];
      }
    }
  }

  if (units <= MAX_FIXED_BLOCK_UNITS) {
    for (int i = 0; i < FIXED_BINS; i++) {
      size_t bin_unit_size =
          MIN_FIXED_BLOCK_UNITS + (i + 1) * FIXED_BLOCK_INTERVAL - 1;
      if (units <= bin_unit_size && fixed_bins[i] != NULL) {
        printf("return fixed address: %p", (void *)&fixed_bins[i]);

        return &fixed_bins[i];
      }
    }
  }
  printf("return large_chunk_free_head address: %p",
         (void *)&large_chunk_free_head);
  return &large_chunk_free_head;
}

/*--------------------------------------------------------------*/
/* size_to_units:
 * Returns capable number of units for 'size' bytes.
 */
/*--------------------------------------------------------------*/
static size_t size_to_units(size_t size) {
  return (size + (CHUNK_UNIT - 1)) / CHUNK_UNIT;
}
/*--------------------------------------------------------------*/
/* get_chunk_from_data_ptr:
 * Returns the header pointer that contains data 'm'.
 */
/*--------------------------------------------------------------*/
static Chunk_T get_chunk_from_data_ptr(void *m) {
  return (Chunk_T)((char *)m - CHUNK_UNIT);
}

/*--------------------------------------------------------------------*/
/* init_my_heap:
 * Initialize data structures and global variables for
 * chunk management.
 */
/*--------------------------------------------------------------------*/
static void init_my_heap(void) {
  /* Initialize bins */
  init_exponential_bins();
  init_fixed_bins();

  /* Initialize g_heap_start and g_heap_end */
  g_heap_start = g_heap_end = sbrk(0);
  if (g_heap_start == (void *)-1) {
    fprintf(stderr, "sbrk(0) failed\n");
    exit(-1);
  }
}
/*--------------------------------------------------------------------*/
/* merge_chunk:
 * merge two adjacent chunk which is not in the free list.
 * before call merge_chunk, c1 and c2 shuuld be removed from list.
 * simply merge and return merged chunk.
 */
/*--------------------------------------------------------------------*/
static Chunk_T merge_chunk(Chunk_T c1, Chunk_T c2) {
  // c1 and c2 must be adjacent
  assert(c1 < c2 &&
         chunk_get_next_adjacent(c1, g_heap_start, g_heap_end) == c2);
  assert(chunk_get_status(c1) == CHUNK_IN_USE);
  assert(chunk_get_status(c2) == CHUNK_IN_USE);

  int merged_units = chunk_get_units(c1) + chunk_get_units(c2) + 2;
  chunk_set_units(c1, merged_units);

  // after resize, footer value should be reallocated
  Chunk_F c1_footer =
      chunk_get_footer_from_header(c1, g_heap_start, g_heap_end);
  assert(c1_footer != NULL);
  chunk_footer_set_units(c1_footer, merged_units);
  chunk_footer_set_prev_free_chunk(c1_footer, NULL);

  return c1;
}
/* split_and_relocate_chunk:
 * 프리 리스트 안에 있는 c를 받아서, c를 프리 리스트에서 제거한 뒤,
 * 두 조각으로 나눈다. units에 해당하는 파트는 리턴하고, 나머지에 해당하는
 * 파트는 새로운 bin에 집어넣는다.
 */
/*--------------------------------------------------------------------*/
static Chunk_T split_and_relocate_remaining_chunk(Chunk_T c, size_t units) {
  Chunk_T c2;
  size_t all_units;

  assert(c >= (Chunk_T)g_heap_start && c <= (Chunk_T)g_heap_end);
  assert(chunk_get_status(c) == CHUNK_FREE);
  assert(chunk_get_units(c) > units + 2);  // consider header and footer

  remove_chunk_from_list(c);

  // adjust the size of the first chunk

  all_units = chunk_get_units(c);

  // resize c and reallocate value to new footer
  chunk_set_units(c, all_units - units - 2);
  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);
  chunk_footer_set_units(c_footer, all_units - units - 2);
  chunk_set_status(c, CHUNK_FREE);
  // relocate c

  insert_chunk(c);

  // prepare for the second chunk
  c2 = chunk_get_next_adjacent(c, g_heap_start, g_heap_end);
  chunk_set_units(c2, units);
  Chunk_F c2_footer =
      chunk_get_footer_from_header(c2, g_heap_start, g_heap_end);
  assert(c2_footer != NULL);
  chunk_footer_set_units(c2_footer, units);
  chunk_set_status(c2, CHUNK_IN_USE);

  return c2;
}

/*--------------------------------------------------------------------*/
/* insert_chunk:
 * insert free chunk c into the first place of free list.
 * simply insert and don't merge any chunk.
 */
/*--------------------------------------------------------------------*/
static void insert_chunk(Chunk_T c) {
  assert(c != NULL);
  assert(chunk_get_units(c) >= 1);
  assert(chunk_get_status(c) == CHUNK_FREE);

  Chunk_T *free_head = find_bin(chunk_get_units(c));
  assert(free_head != NULL);

  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);
  /*
    If the free chunk list is empty, set next and prev as NULL.
    else simply set c as header of free list.
  */
  if (*free_head == NULL) {
    *free_head = c;
    chunk_set_next_free_chunk(c, NULL);
    chunk_footer_set_prev_free_chunk(c_footer, NULL);
  } else {
    chunk_set_next_free_chunk(c, *free_head);
    chunk_footer_set_prev_free_chunk(c_footer, NULL);

    Chunk_F head_footer =
        chunk_get_footer_from_header(*free_head, g_heap_start, g_heap_end);
    assert(head_footer != NULL);

    chunk_footer_set_prev_free_chunk(head_footer, c);
    *free_head = c;
  }
}

/*--------------------------------------------------------------------*/
/* remove_chunk_from_list:
 * remove free chunk c from free list and mark it as CHUNK_IN_USE.
 */
/*--------------------------------------------------------------------*/
static void remove_chunk_from_list(Chunk_T c) {
  assert(c != NULL);
  assert(chunk_get_status(c) == CHUNK_FREE);

  Chunk_T *free_head = find_bin(chunk_get_units(c));
  assert(free_head != NULL);
  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);

  Chunk_T prev = chunk_footer_get_prev_free_chunk(c_footer);
  Chunk_T next = chunk_get_next_free_chunk(c);
  Chunk_F next_footer;

  if (prev == NULL && next == NULL) {
    // case of length = 1: simply set head as NULL
    *free_head = NULL;
  } else if (prev == NULL) {
    // case of c is first element: change next chunk's header and foooter.
    next_footer = chunk_get_footer_from_header(next, g_heap_start, g_heap_end);
    assert(next_footer != NULL);

    chunk_footer_set_prev_free_chunk(next_footer, NULL);
    *free_head = next;
  } else if (next == NULL) {
    // case of c is last element: change prev chunk's header as pointing NULL
    chunk_set_next_free_chunk(prev, NULL);
  } else {
    /*
        else: connect prev and next by changing their header and footer to point
        each other.
    */
    next_footer = chunk_get_footer_from_header(next, g_heap_start, g_heap_end);
    assert(next_footer != NULL);

    chunk_footer_set_prev_free_chunk(next_footer, prev);
    chunk_set_next_free_chunk(prev, next);
  }

  chunk_set_next_free_chunk(c, NULL);
  chunk_footer_set_prev_free_chunk(c_footer, NULL);
  chunk_set_status(c, CHUNK_IN_USE);
}
/*--------------------------------------------------------------------*/
/* allocate_more_memory:
 * requests additional memory from the system using `sbrk()`.
 * Allocates `units` chunk units, plus space for a header and footer.
 * if units is smaller than MEMALLOC_MIN, use MEMALLOC_MIN.
 * after allocated, initialize its header and footer and put it to free list.
 */
/*--------------------------------------------------------------------*/
static Chunk_T allocate_more_memory(size_t units) {
  Chunk_T c;

  if (units < MEMALLOC_MIN) units = MEMALLOC_MIN;

  /* Note that we need to allocate one more unit for header. */
  c = (Chunk_T)sbrk((units + 2) * CHUNK_UNIT);
  if (c == (Chunk_T)-1) return NULL;

  g_heap_end = sbrk(0);
  chunk_set_units(c, units);
  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);
  chunk_footer_set_units(c_footer, units);
  chunk_footer_set_prev_free_chunk(c_footer, NULL);
  chunk_set_status(c, CHUNK_FREE);

  return c;
}
/*--------------------------------------------------------------*/
/* heapmgr_malloc:
 * Dynamically allocate a memory capable of holding size bytes.
 * Substitute for GNU malloc().
 */
/*--------------------------------------------------------------*/
void *heapmgr_malloc(size_t size) {
  static int is_init = FALSE;
  Chunk_T c;
  size_t units;
  if (size <= 0) return NULL;

  if (is_init == FALSE) {
    init_my_heap();
    is_init = TRUE;
  }

  /* see if everything is OK before doing any operations */
  assert(check_heap_validity());

  units = size_to_units(size);

  printf("==========\nmalloc started\n");

  Chunk_T *free_head = find_best_fit_bin(units);

  printf("=====\n freehead add: %p %d's add val: %p \n", (void *)free_head,
         *free_head ? chunk_get_units(*free_head) : 63,
         (void *)find_bin(*free_head ? chunk_get_units(*free_head) : 63));

  if (*free_head != NULL) {
    printf("get free head\n");

    c = *free_head;  // => 이게 문제였음. 결국 for문을 돌긴 해야하는디...ㅄ
    // if (chunk_get_units(c) >= units) {
    //   printf("=====\n %d %d \n", chunk_get_units(c), (int)units);
    // }
    assert(chunk_get_units(c) >= units);
    if (chunk_get_units(c) > units + 2)
      c = split_and_relocate_remaining_chunk(c, units);

    assert(chunk_get_status(c) == CHUNK_IN_USE);

    assert(check_heap_validity());
    return (void *)((char *)c + CHUNK_UNIT);
  }

  // case of there's no fit free chunk in list

  // allocate new memory
  printf("no free head\n");

  c = allocate_more_memory(units);

  if (c == NULL) {
    assert(check_heap_validity());
    return NULL;
  }

  assert(chunk_get_units(c) >= units);

  /*
    if splitable, split and change c as splitted second chunk from
    split_chunk
  */
  if (chunk_get_units(c) > units + 2) {
    size_t c_units = chunk_get_units(c);
    chunk_set_units(c, units);
    Chunk_F c_footer =
        chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
    assert(c_footer != NULL);
    chunk_footer_set_units(c_footer, units);

    Chunk_T c2 = chunk_get_next_adjacent(c, g_heap_start, g_heap_end);
    chunk_set_units(c2, c_units - units - 2);
    Chunk_F c2_footer =
        chunk_get_footer_from_header(c2, g_heap_start, g_heap_end);
    assert(c2_footer != NULL);
    chunk_footer_set_units(c2_footer, c_units - units - 2);
    chunk_set_status(c2, CHUNK_FREE);
    insert_chunk(c2);
  }

  // remove chunk with size of units from free list.
  chunk_set_status(c, CHUNK_IN_USE);

  assert(check_heap_validity());
  return (void *)((char *)c + CHUNK_UNIT);
}
/*--------------------------------------------------------------*/
/* heapmgr_free:
 * Releases dynamically allocated memory.
 * Substitute for GNU free().                                   */
/*--------------------------------------------------------------*/
void heapmgr_free(void *m) {
  Chunk_T c;

  if (m == NULL) return;

  // check everything is OK before freeing 'm'
  assert(check_heap_validity());

  // get the chunk header pointer from m
  c = get_chunk_from_data_ptr(m);
  assert((void *)c >= g_heap_start && (void *)c < g_heap_end);
  assert(chunk_get_status(c) != CHUNK_FREE);

  // get next and prev header of c
  Chunk_T next = chunk_get_next_adjacent(c, g_heap_start, g_heap_end);
  Chunk_T prev = chunk_get_prev_adjacent(c, g_heap_start, g_heap_end);

  // if next adjacent chunk is free, remove it from free list and merge with c.
  if (next != NULL && chunk_get_status(next) == CHUNK_FREE) {
    remove_chunk_from_list(next);
    c = merge_chunk(c, next);
  }

  // if prev adjacent chunk is free, remove it from free list and merge with c.
  if (prev != NULL && chunk_get_status(prev) == CHUNK_FREE) {
    remove_chunk_from_list(prev);
    c = merge_chunk(prev, c);
  }

  // now set merged(or not) c as CHUNK_FREE, and put it to free list.
  chunk_set_status(c, CHUNK_FREE);
  insert_chunk(c);

  /* double check if everything is OK */
  assert(check_heap_validity());
}