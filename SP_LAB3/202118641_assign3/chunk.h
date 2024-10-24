/*--------------------------------------------------------------------*/
/* Chunk.h, originated from chunkbase.h */
/* Author: Lee, Ha Dong */
/* Original author: Donghwi Kim, KyoungSoo Park */
/*--------------------------------------------------------------------*/

#ifndef _CHUNK_BASE_H_
#define _CHUNK_BASE_H_

#pragma once

#include <stdbool.h>
#include <unistd.h>

/*
   Each chunk has a header and footer and data units. Each header in a chunk has
   a pointer to the next chunk in the free list. And each footer in a chunk has
   a pointer to the prev chunk in the free list. The size of a chunk is in the
   number of units, but it does not include the unit for a header and footer.
   For example, if a chunk has 5 units, its actual size is 7 units since it has
   one header and one footer.
*/

typedef struct Chunk *Chunk_T;
typedef struct Chunk_Footer *Chunk_F;

enum {
  CHUNK_FREE,
  CHUNK_IN_USE,
};

enum {
  CHUNK_UNIT = 16, /* 16 = sizeof(struct Chunk) */
};

/* chunk_get_status:
 * Returns a chunk's status which shows whether the chunk is in use or free.
 * Return value is either CHUNK_IN_USE or CHUNK_FREE. */
int chunk_get_status(Chunk_T c);

/* chunk_set_status:
 * Set the status of the chunk, 'c'.
 * status can be either CHUNK_FREE or CHUNK_IN_USE */
void chunk_set_status(Chunk_T c, int status);

/* chunk_get_units:
 * Returns the size of a chunk, 'c', in terms of the number of chunk units. */
int chunk_get_units(Chunk_T c);

/* chunk_set_units:
 * Sets the current size in 'units' of 'c' */
void chunk_set_units(Chunk_T c, int units);

/* chunk_get_next_free_chunk:
 * Returns the next free chunk in free chunk list.
 * Returns NULL if 'c' is the last free chunk in the list. */
Chunk_T chunk_get_next_free_chunk(Chunk_T c);

/* chunk_set_next_free_chunk:
 * Sets the next free chunk of 'c' to 'next') */
void chunk_set_next_free_chunk(Chunk_T c, Chunk_T next);

/* chunk_footer_get_status:
 * Returns the status of the footer 'c'. Note that status is not used
 * in the current code but can be used for future extensions. */
int chunk_footer_get_status(Chunk_F c);

/* chunk_footer_set_status:
 * Sets the status of the footer 'c' to 'status'. */
void chunk_footer_set_status(Chunk_F c, int status);

/* chunk_footer_get_units:
 * Returns the size (in units) of the chunk associated with footer 'c'. */
int chunk_footer_get_units(Chunk_F c);

/* chunk_footer_set_units:
 * Sets the size of the chunk associated with footer 'c' to 'units'. */
void chunk_footer_set_units(Chunk_F c, int units);

/* chunk_footer_get_prev_free_chunk:
 * Returns the previous chunk in the free chunk list, as stored in
 * the footer 'c'. */
Chunk_T chunk_footer_get_prev_free_chunk(Chunk_F c);

/* chunk_footer_set_prev_free_chunk:
 * Sets the previous free chunk for the footer 'c' to 'prev'. */
void chunk_footer_set_prev_free_chunk(Chunk_F c, Chunk_T prev);

/* chunk_get_next_adjacent:
 * Returns the next adjacent chunk to 'c' in memory. 'start' is the
 * heap start, and 'end' is the heap end. Returns NULL if 'c' is the last
 * chunk in the heap. */
Chunk_T chunk_get_next_adjacent(Chunk_T c, void *start, void *end);

/* chunk_get_prev_adjacent:
 * Returns the previous adjacent chunk to 'c' in memory, using the
 * footer of the preceding chunk. 'start' is the heap start, and 'end'
 * is the heap end. Returns NULL if 'c' is the first chunk in the heap. */
Chunk_T chunk_get_prev_adjacent(Chunk_T c, void *start, void *end);

/* chunk_get_header_from_footer:
 * Given a footer 'c', returns the header of the corresponding chunk.
 * 'start' is the heap start, and 'end' is the heap end. Returns NULL
 * if the header is outside of the heap bounds. */
Chunk_T chunk_get_header_from_footer(Chunk_F c, void *start, void *end);

/* chunk_get_footer_from_header:
 * Given a chunk header 'c', returns the footer of the corresponding chunk.
 * 'start' is the heap start, and 'end' is the heap end. Returns NULL
 * if the footer is outside of the heap bounds. */
Chunk_F chunk_get_footer_from_header(Chunk_T c, void *start, void *end);

/* Following two functions are for debugging.
 * These will be removed by C preprocessor if you compile the code with
 * -DNDEBUG option.
 */
#ifndef NDEBUG
/* chunk_is_valid:
 * Checks the validity of a chunk, 'c'. Returns 1 if OK otherwise 0.
 * start is the pointer to the start of the Heap
 * end is the the pointer to the end of the Heap
 */
int chunk_is_valid(Chunk_T c, void *start, void *end);

#endif /* NDEBUG */

#endif /* _CHUNK_BASE_H_ */