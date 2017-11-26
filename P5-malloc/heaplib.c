#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// -----------DEFINITIONS-----------------/

// header info for each block of memory we're managing
// we'll be using these in doubly linked lists
typedef struct _block_info_t {
    unsigned int block_size;
    struct _block_info_t *prev_block;
    struct _block_info_t *next_block;
    void *data;
} block_info_t;

// main struct for top level mem manager info
typedef struct _heap_header_t {
    unsigned int heap_size;
    block_info_t *first_free_block;
    block_info_t *first_used_block;
} heap_header_t ;

/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))


/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First sanity check the inputs. Exit(1) if obviously crap.
 *
 *
 */
void hl_init(void *heap, unsigned int heap_size) {

	// this would be our non-graceful failure for bad input
	if (heap == NULL || heap_size < MIN_HEAP_SIZE) {
		exit(1);
	}

	// so when we start, we have 0 used blocks,
	// and one free block with size of all available mem
	// this top level struct goes at base of our allocated mem
	heap_header_t *hdr = (heap_header_t *) heap;
	hdr->first_used_block = NULL;

	hdr->first_free_block = (block_info_t *) ADD_BYTES(heap, sizeof(heap_header_t));
	hdr->first_free_block->block_size = heap_size - sizeof(heap_header_t) - sizeof(block_info_t);
	hdr->first_free_block->prev_block = NULL;
	hdr->first_free_block->next_block = NULL;
	hdr->first_free_block->data = ADD_BYTES(hdr->first_free_block, sizeof(block_info_t));

    return;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_alloc(void *heap, unsigned int block_size) {

    return NULL;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void hl_release(void *heap, void *block) {

}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_resize(void *heap, void *block, unsigned int new_size) {

    return NULL;
}

