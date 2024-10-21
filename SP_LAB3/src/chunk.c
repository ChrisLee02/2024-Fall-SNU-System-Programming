/*--------------------------------------------------------------------*/
/* Chunk.c, originated from chunkbase.c */
/* Author: Lee, Ha Dong */
/* Original author: Donghwi Kim, KyoungSoo Park */
/*--------------------------------------------------------------------*/

#include "chunk.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Chunk {
  Chunk_T next; /* Pointer to the next chunk in the free chunk list */
  int units;    /* Capacity of a chunk (chunk units) */
  int status;   /* CHUNK_FREE or CHUNK_IN_USE */
};

struct Chunk_Footer {
  Chunk_T prev; /* Pointer to the prev chunk in the free chunk list */
  int units;    /* Capacity of a chunk (chunk units) */
  int status;   /* CHUNK_FREE or CHUNK_IN_USE, not used in code.*/
};

/*--------------------------------------------------------------------*/
int chunk_get_status(Chunk_T c) { return c->status; }
/*--------------------------------------------------------------------*/
void chunk_set_status(Chunk_T c, int status) { c->status = status; }
/*--------------------------------------------------------------------*/
int chunk_get_units(Chunk_T c) { return c->units; }
/*--------------------------------------------------------------------*/
void chunk_set_units(Chunk_T c, int units) { c->units = units; }
/*--------------------------------------------------------------------*/
Chunk_T chunk_get_next_free_chunk(Chunk_T c) { return c->next; }
/*--------------------------------------------------------------------*/
void chunk_set_next_free_chunk(Chunk_T c, Chunk_T next) { c->next = next; }
/*--------------------------------------------------------------------*/
int chunk_footer_get_status(Chunk_F c) { return c->status; }
/*--------------------------------------------------------------------*/
void chunk_footer_set_status(Chunk_F c, int status) { c->status = status; }
/*--------------------------------------------------------------------*/
int chunk_footer_get_units(Chunk_F c) { return c->units; }
/*--------------------------------------------------------------------*/
void chunk_footer_set_units(Chunk_F c, int units) { c->units = units; }
/*--------------------------------------------------------------------*/
Chunk_T chunk_footer_get_prev_free_chunk(Chunk_F c) { return c->prev; }
/*--------------------------------------------------------------------*/
void chunk_footer_set_prev_free_chunk(Chunk_F c, Chunk_T prev) {
  c->prev = prev;
}
/*--------------------------------------------------------------------*/
Chunk_T chunk_get_next_adjacent(Chunk_T c, void *start, void *end) {
  Chunk_T n;

  assert((void *)c >= start);

  /* Note that a chunk consists of one chunk unit for a header, and
   * many chunk units for data. */
  n = c + c->units + 2;

  /* If 'c' is the last chunk in memory space, then return NULL. */
  if ((void *)n >= end) return NULL;

  return n;
}

/*--------------------------------------------------------------------*/
Chunk_T chunk_get_prev_adjacent(Chunk_T c, void *start, void *end) {
  assert((void *)c <= end);

  Chunk_F prev_footer = (Chunk_F)(c - 1);

  /* If 'c' is the first chunk in memory space, then return NULL. */
  if ((void *)prev_footer < start) return NULL;

  Chunk_T n;

  n = (Chunk_T)(prev_footer - prev_footer->units - 1);
  if ((void *)n < start) return NULL;
  return n;
}
/*--------------------------------------------------------------------*/
Chunk_T chunk_get_header_from_footer(Chunk_F c, void *start, void *end) {
  assert((void *)c <= end);
  assert((void *)c >= start);

  Chunk_T n;
  n = (Chunk_T)(c - c->units - 1);
  if ((void *)n < start) return NULL;
  return n;
}
/*--------------------------------------------------------------------*/
Chunk_F chunk_get_footer_from_header(Chunk_T c, void *start, void *end) {
  assert((void *)c <= end);
  assert((void *)c >= start);

  Chunk_F n;
  n = (Chunk_F)(c + c->units + 1);
  if ((void *)n >= end) return NULL;
  return n;
}

#ifndef NDEBUG
/*--------------------------------------------------------------------*/
int chunk_is_valid(Chunk_T c, void *start, void *end)
/* Return 1 (TRUE) iff c is valid */
{
  assert(c != NULL);
  assert(start != NULL);
  assert(end != NULL);

  if (c < (Chunk_T)start) {
    fprintf(stderr, "Bad heap start\n");
    return 0;
  }
  if (c >= (Chunk_T)end) {
    fprintf(stderr, "Bad heap end\n");
    return 0;
  }
  if (c->units == 0) {
    fprintf(stderr, "Zero units\n");
    return 0;
  }
  return 1;
}
#endif