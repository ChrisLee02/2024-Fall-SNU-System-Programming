/*--------------------------------------------------------------------*/
/* heapmgr1.c                                                      */
/* Author: Lee, Ha Dong                                */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"

#define FALSE 0
#define TRUE 1

enum {
  MEMALLOC_MIN = 1024,
};

/* g_free_head: point to first chunk in the free list */
static Chunk_T g_free_head = NULL;
int sbrk_cnt = 0;
int malloc_cnt = 0;
/* g_heap_start, g_heap_end: start and end of the heap area.
 * g_heap_end will move if you increase the heap */
static void *g_heap_start = NULL, *g_heap_end = NULL;
int max_free_block_units = 0;

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

  if (g_heap_start == g_heap_end) {
    if (g_free_head == NULL) return 1;
    fprintf(stderr, "Inconsistent empty heap\n");
    return FALSE;
  }

  for (w = (Chunk_T)g_heap_start; w && w < (Chunk_T)g_heap_end;
       w = chunk_get_next_adjacent(w, g_heap_start, g_heap_end)) {
    if (!chunk_is_valid(w, g_heap_start, g_heap_end)) return 0;
  }

  for (w = g_free_head; w; w = chunk_get_next_free_chunk(w)) {
    if (chunk_get_status(w) != CHUNK_FREE) {
      fprintf(stderr, "Non-free chunk in the free chunk list\n");
      return 0;
    }

    if (!chunk_is_valid(w, g_heap_start, g_heap_end)) return 0;
  }
  return TRUE;
}

#endif

void print_all(void) {
  Chunk_T w;
  printf("[DEBUG] Checking all chunks in the heap...\n");

  // Iterate through all chunks in the heap
  for (w = (Chunk_T)g_heap_start; w && w < (Chunk_T)g_heap_end;
       w = chunk_get_next_adjacent(w, g_heap_start, g_heap_end)) {
    printf("[DEBUG] Chunk at %p: Status = %s, Units = %d\n", (void *)w,
           (chunk_get_status(w) == CHUNK_FREE ? "FREE" : "IN USE"),
           chunk_get_units(w));
  }

  printf("[DEBUG] Checking all chunks in the free list...\n");

  // Iterate through the free list and print chunks
  for (w = g_free_head; w; w = chunk_get_next_free_chunk(w)) {
    printf("[DEBUG] Free chunk at %p: Units = %d\n", (void *)w,
           chunk_get_units(w));
  }
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
  /* Initialize g_heap_start and g_heap_end */
  g_heap_start = g_heap_end = sbrk(0);
  if (g_heap_start == (void *)-1) {
    fprintf(stderr, "sbrk(0) failed\n");
    exit(-1);
  }
}
/*--------------------------------------------------------------------*/
/* merge_chunk:
 * 리스트에서 제거된 두 개의 청크를 가져와서 병합,
 * 단순히 병합된 하나의 청크를 리턴한다.
 */
/*--------------------------------------------------------------------*/
static Chunk_T merge_chunk(Chunk_T c1, Chunk_T c2) {
  /* c1 and c2 must be be adjacent */
  assert(c1 < c2 &&
         chunk_get_next_adjacent(c1, g_heap_start, g_heap_end) == c2);
  assert(chunk_get_status(c1) == CHUNK_IN_USE);
  assert(chunk_get_status(c2) == CHUNK_IN_USE);

  int merged_units = chunk_get_units(c1) + chunk_get_units(c2) + 2;
  chunk_set_units(c1, merged_units);

  Chunk_F c1_footer =
      chunk_get_footer_from_header(c1, g_heap_start, g_heap_end);
  assert(c1_footer != NULL);
  chunk_footer_set_units(c1_footer, merged_units);
  chunk_footer_set_prev_free_chunk(c1_footer, NULL);

  return c1;
}
/*--------------------------------------------------------------------*/
/* split_chunk:
 * c를 받아서 c와 c2로 나눔, c2에 units만큼을 할당한다.
 * 리스트에서 제거하지 않고, 두 개의 노드로 나누기만 한다.
 */
/*--------------------------------------------------------------------*/
static Chunk_T split_chunk(Chunk_T c, size_t units) {
  Chunk_T c2, c3;
  size_t all_units;

  assert(c >= (Chunk_T)g_heap_start && c <= (Chunk_T)g_heap_end);
  assert(chunk_get_status(c) == CHUNK_FREE);
  assert(chunk_get_units(c) > units + 2); /* assume chunk with header unit */

  /* adjust the size of the first chunk */

  c3 = chunk_get_next_free_chunk(c);

  // c를 재할당
  all_units = chunk_get_units(c);

  Chunk_F c_footer_old =
      chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer_old != NULL);
  Chunk_T c_prev = chunk_footer_get_prev_free_chunk(c_footer_old);

  chunk_set_units(c, all_units - units - 2);

  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);
  chunk_footer_set_units(c_footer, all_units - units - 2);
  chunk_footer_set_prev_free_chunk(c_footer, c_prev);

  /* prepare for the second chunk */
  c2 = chunk_get_next_adjacent(c, g_heap_start, g_heap_end);
  chunk_set_units(c2, units);
  Chunk_F c2_footer =
      chunk_get_footer_from_header(c2, g_heap_start, g_heap_end);
  assert(c2_footer != NULL);
  chunk_footer_set_prev_free_chunk(c2_footer, c);
  chunk_footer_set_units(c2_footer, units);

  chunk_set_status(c2, CHUNK_FREE);
  chunk_set_next_free_chunk(c2, c3);

  chunk_set_next_free_chunk(c, c2);

  if (c3 != NULL) {
    Chunk_F c3_footer =
        chunk_get_footer_from_header(c3, g_heap_start, g_heap_end);

    assert(c3_footer != NULL);
    chunk_footer_set_prev_free_chunk(c3_footer, c2);
  }
  return c2;
}
/*--------------------------------------------------------------------*/
/* insert_chunk:
 * 여기서는 FREE인 청크를 받아서 삽입만 한다.
 * 더 이상 리스트는 인접성과는 관련이 없음.
 */
/*--------------------------------------------------------------------*/
static void insert_chunk(Chunk_T c) {
  assert(c != NULL);
  assert(chunk_get_units(c) >= 1);
  assert(chunk_get_status(c) == CHUNK_FREE);

  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);
  /* If the free chunk list is empty, chunk c points to itself. */
  if (g_free_head == NULL) {
    g_free_head = c;
    chunk_set_next_free_chunk(c, NULL);
    chunk_footer_set_prev_free_chunk(c_footer, NULL);
  } else {
    chunk_set_next_free_chunk(c, g_free_head);
    chunk_footer_set_prev_free_chunk(c_footer, NULL);

    Chunk_F head_footer =
        chunk_get_footer_from_header(g_free_head, g_heap_start, g_heap_end);
    assert(head_footer != NULL);

    chunk_footer_set_prev_free_chunk(head_footer, c);
    g_free_head = c;
  }
}

/*--------------------------------------------------------------------*/
/* remove_chunk_from_list:
 * 리스트에서 제거 후, in use로 상태를 변경한다.
 */
/*--------------------------------------------------------------------*/
static void remove_chunk_from_list(Chunk_T c) {
  assert(c != NULL);
  assert(chunk_get_status(c) == CHUNK_FREE);
  Chunk_F c_footer = chunk_get_footer_from_header(c, g_heap_start, g_heap_end);
  assert(c_footer != NULL);

  Chunk_T prev = chunk_footer_get_prev_free_chunk(c_footer);
  Chunk_T next = chunk_get_next_free_chunk(c);
  Chunk_F next_footer;
  if (prev == NULL && next == NULL) {
    g_free_head = NULL;
  } else if (prev == NULL) {
    next_footer = chunk_get_footer_from_header(next, g_heap_start, g_heap_end);
    assert(next_footer != NULL);

    chunk_footer_set_prev_free_chunk(next_footer, NULL);
    g_free_head = next;
  } else if (next == NULL) {
    chunk_set_next_free_chunk(prev, NULL);
  } else {
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
 * 힙을 더 떼오고,
 * 떼온걸 통채로 자유 리스트에 넣은 뒤 단일 블록으로 만들어 반환한다.
 *
 */
/*--------------------------------------------------------------------*/
static Chunk_T allocate_more_memory(size_t units) {
  // if (sbrk_cnt % 1000 == 0)
  //   printf("sbrk: %d times\n", ++sbrk_cnt);
  // else
  //   sbrk_cnt++;
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
  // printf("allocate 직후:\n");
  // print_all();

  insert_chunk(c);

  return c;
}
/*--------------------------------------------------------------*/
/* heapmgr_malloc:
 * Dynamically allocate a memory capable of holding size bytes.
 * Substitute for GNU malloc().
 */
/*--------------------------------------------------------------*/
void *heapmgr_malloc(size_t size) {
  // printf("malloc: %d times\n", ++malloc_cnt);
  // print_all();
  // if (malloc_cnt % 1000 == 0) {
  //   printf("malloc: %d times\n", ++malloc_cnt);
  //   if ((malloc_cnt - 1) % 10000 == 0) {
  //     print_all();
  //   }
  // } else {
  //   malloc_cnt++;
  // }
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

  // printf("malloc: units: %d\n", (int)(units));

  for (c = g_free_head; c != NULL; c = chunk_get_next_free_chunk(c)) {
    if (chunk_get_units(c) >= units) {
      if (chunk_get_units(c) > units + 2) c = split_chunk(c, units);

      remove_chunk_from_list(c);

      assert(check_heap_validity());
      return (void *)((char *)c + CHUNK_UNIT);
    }
  }

  /* allocate new memory */
  c = allocate_more_memory(units);

  if (c == NULL) {
    assert(check_heap_validity());
    return NULL;
  }

  // todo: allocate은 진짜 할당 후 자유 리스트에 넣기만 할거임.
  // todo: 여기서 c에 대한 로직을 작성한다.
  // todo: c는 더 떼온 추가 할당분과 FREE 속성만을 갖고 있음.
  // todo: 크기 split하는 로직 넣고.

  assert(chunk_get_units(c) >= units);

  // printf("두 개 프린트: %d %d\n", chunk_get_units(c), (int)units);

  if (chunk_get_units(c) > units + 2) c = split_chunk(c, units);

  remove_chunk_from_list(c);

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

  /* check everything is OK before freeing 'm' */
  assert(check_heap_validity());

  /* get the chunk header pointer from m */
  c = get_chunk_from_data_ptr(m);
  assert((void *)c >= g_heap_start && (void *)c < g_heap_end);
  assert(chunk_get_status(c) != CHUNK_FREE);

  // todo: 양옆을 확인하고 병합 시도, 성공 시 해당 청크 하나를 c로 재할당함.
  // todo: 병합 시에는 기존 리스트에서 제거 후에 병합하고, 맨 앞으로 넣는다.
  // 그게 깔끔함. todo: c를 맨 앞으로 insert

  Chunk_T next = chunk_get_next_adjacent(c, g_heap_start, g_heap_end);
  Chunk_T prev = chunk_get_prev_adjacent(c, g_heap_start, g_heap_end);
  if (next != NULL && chunk_get_status(next) == CHUNK_FREE) {
    remove_chunk_from_list(next);

    // @debug
    // if (chunk_get_next_adjacent(c, g_heap_start, g_heap_end) != next) {
    //   printf("\n[DEBUG] 씨@발1, %p\n", NULL);
    //   printf("\n[DEBUG] Merging chunks: %p and %p\n", (void *)c, (void
    //   *)next); printf("[DEBUG] Chunk 1 units: %d, Chunk 2 units: %d\n",
    //          chunk_get_units(c), chunk_get_units(next));

    //   fflush(stdout);

    //   print_all();
    //   exit(0);
    // }

    c = merge_chunk(c, next);
  }

  if (prev != NULL && chunk_get_status(prev) == CHUNK_FREE) {
    remove_chunk_from_list(prev);

    // @debug
    // if (chunk_get_next_adjacent(prev, g_heap_start, g_heap_end) != c) {
    //   printf("\n[DEBUG] 씨@발2,\n");
    //   printf("\n[DEBUG] Merging chunks: %p and %p\n", (void *)prev, (void
    //   *)c); printf("[DEBUG] Chunk 1 units: %d, Chunk 2 units: %d\n",
    //          chunk_get_units(prev), chunk_get_units(c));

    //   fflush(stdout);

    //   print_all();
    //   exit(0);
    // }

    c = merge_chunk(prev, c);
  }
  chunk_set_status(c, CHUNK_FREE);
  insert_chunk(c);

  /* double check if everything is OK */
  assert(check_heap_validity());
}