#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// -----------DEFINITIONS-----------------

// header info for each block of memory we're managing
// we'll be using these in linked lists

typedef struct _block_info_t {
    unsigned int data_size; // amount of user data--doesn't include block info
    //struct _block_info_t *prev_block;
    struct _block_info_t *next_block;
    //char *data; // this is the pointer to the user data
} block_info_t;

// main struct for top level mem manager info

typedef struct _heap_header_t {
    unsigned int heap_size;
    block_info_t *first_free_block;
    block_info_t *first_used_block;
} heap_header_t;

/* Useful shorthand: casts a pointer to a (char *) before adding/subtracting */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))
#define SUB_BYTES(base_addr, num_bytes) (((char *)(base_addr)) - (num_bytes))

/* Given a target address, returns the next properly aligned address. same if
 * input address is already aligned.
 */
#define ALIGN_UP(addr) ADD_BYTES((addr), ((ALIGNMENT - (((uintptr_t)(addr)) % ALIGNMENT)) % ALIGNMENT))
#define ALIGN_UP_2(addr) ((char*) (((((uintptr_t) (addr)) + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT))

/* we only want to actually use block data sizes that are even multiples of
 * ALIGNMENT, so this rounds up a size to be a multiple of ALIGNMENT if needed.
 */
#define ALIGNED_SIZE(nbytes) ( (((nbytes) + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT )

/* Given a ptr to a block_data_t, returns address where the actual data starts
 */
#define BLOCK_DATA(block_info_ptr) ADD_BYTES(block_info_ptr, sizeof(block_info_t))

/* -------------------- PRINT DEBUG FNS ----------------- */

#ifdef PRINT_DEBUG
#define DEBUG_PRINT(msg) printf(msg)
#define DEBUG_PRINT_1(fmt, v1) printf(fmt, v1)
#define DEBUG_PRINT_2(fmt, v1, v2) printf(fmt, v1, v2)
#define DEBUG_PRINT_3(fmt, v1, v2, v3) printf(fmt, v1, v2, v3)
#else
#define DEBUG_PRINT(msg) printf(msg)
#define DEBUG_PRINT_1(fmt, v1)
#define DEBUG_PRINT_2(fmt, v1, v2)
#define DEBUG_PRINT_3(fmt, v1, v2, v3)
#endif

// printf some overall heap info

void print_debug_heap_state(char *msg, heap_header_t *hdr) {
#ifdef PRINT_DEBUG
    // intro msg, then size and addr

    printf("%s--Heap addr: %p; Heap size: %u; ", msg, hdr, hdr->heap_size);

    // count free blocks and find largest
    int n_blocks = 0;
    unsigned int biggest_block = 0;
    block_info_t *check_block = hdr->first_free_block;
    while (check_block != NULL) {
        n_blocks++;
        if (check_block->data_size > biggest_block) {
            biggest_block = check_block->data_size;
        }
        check_block = check_block->next_block;
    }
    printf("n free: %d; biggest free: %u; ", n_blocks, biggest_block);

    // count used blocks and find largest
    n_blocks = 0;
    biggest_block = 0;
    check_block = hdr->first_used_block;
    while (check_block != NULL) {
        n_blocks++;
        if (check_block->data_size > biggest_block) {
            biggest_block = check_block->data_size;
        }
        check_block = check_block->next_block;
    }
    printf("n used: %d; biggest used: %u \n", n_blocks, biggest_block);

#endif
}



//------HELPER FUNCTIONS----------------------

// given a user data block pointer, try to find the block_info
// that goes with the pointer. return NULL for bogus values.
//

block_info_t *get_block_info_for(heap_header_t *hdr, void *block) {
    // return NULL immediately for obviously bogus pointer
    if (((char *) block) < ADD_BYTES(hdr, sizeof (heap_header_t)) ||
            ((char *) block) >= ADD_BYTES(hdr, hdr->heap_size)) {
        return NULL;
    }

    // OK, might be legit. For now just do fast, easy way.
    // Will cause violent death if ptr is bogus though.
    // Maybe be more careful by doing a search?
    return (block_info_t*) SUB_BYTES(block, sizeof (block_info_t));
}


/* this removes the given free block from the list of
 * free blocks in hdr. does nothing if either arg is null.
 */
/*
 void remove_from_free_list(heap_header_t *hdr, block_info_t *remove_this) {

    // do nothing for sketchy inputs
    if (hdr == NULL || remove_this == NULL) return;

    // if it's the head of the list, its next block is now the head
    if (remove_this == hdr->first_free_block) {
        hdr->first_free_block = remove_this->next_block;
        if (hdr->first_free_block != NULL) {
            //hdr->first_free_block->prev_block = NULL;
        }
    } else {
        //normal case is to adjust links to 
        // skip over remove_this
        if (remove_this->prev_block != NULL) {
            remove_this->prev_block->next_block = remove_this->next_block;
        }
        if (remove_this->next_block != NULL) {
            remove_this->next_block->prev_block = remove_this->prev_block;
        }
    }
}
 */

/* this removes the given free block from the list of
 * used blocks in hdr. does nothing if either arg is null or 
 * if it can't find the specified block
 */
void remove_from_used_list(heap_header_t *hdr, void *block_data) {

    // do nothing for sketchy inputs
    if (hdr == NULL || block_data == NULL) return;

    // find the indicated block of memory
    block_info_t *check_block = hdr->first_used_block;
    block_info_t *prev = NULL;

    while (check_block != NULL && BLOCK_DATA(check_block) != block_data) {
        prev = check_block;
        check_block = check_block->next_block;
    }

    // see if we found the block_info for this block_data
    if (check_block != NULL) {

        // if it's the head of the list, its next block is now the head
        if (check_block == hdr->first_used_block) {
            hdr->first_used_block = check_block->next_block;
        } else {
            // normal case: make link from prev skip over this block
            prev->next_block = check_block->next_block;
        }

    }
}

/* this adds the given free block to the list of
 * free blocks in hdr. 
 * maintains list in lo->hi address order. when adding 
 * a block to the list, we check to see if it can be merged into 
 * a bigger free block with the next and/or previous blocks
 * in the list.
 */
void add_to_free_list(heap_header_t *hdr, block_info_t *add_this) {
    DEBUG_PRINT_2("Add to free. add_this: %p, size: %d\n", add_this, add_this->data_size);

    // let's do super easy case first--the list is empty
    block_info_t *check_this = hdr->first_free_block;
    if (check_this == NULL) {
        hdr->first_free_block = add_this;
        add_this->next_block = NULL;
    } else {
        // ok, there's at least one block in the free list.
        // walk down the list until we find a block with
        // a higher address than add_this, or get to the end. 
        // insert there.
        block_info_t *prev = NULL;
        while (check_this->next_block != NULL && check_this < add_this) {
            prev = check_this;
            check_this = check_this->next_block;
        }
        // when we get here, check_this is the last in the 
        // chain and/or check_list is at a higher addr than add_this
        if (check_this < add_this) {
            // insert add_this after check_this
            add_this->next_block = check_this->next_block;
            check_this->next_block = add_this;

        } else {
            // insert add_this before check_this
            add_this->next_block = check_this;

            if (prev == NULL) {
                // add_this is the new head of the list
                hdr->first_free_block = add_this;
            } else {
                // if there was a previous block, set its next to add_this
                prev->next_block = add_this;
            }
        }

        // when we get here, add_this has been put in its correct location
        // in the list. now see if we can merge it with previous or next
        // free blocks
        //
        block_info_t *merge_block = add_this;

        // try merge with previous
        //
        if (prev != NULL) {
            if ((char*) add_this == ADD_BYTES(BLOCK_DATA(prev), prev->data_size)) {

                // merge add_this with previous
                merge_block = prev;
                DEBUG_PRINT_2("merge prev: %p size %d ", add_this, add_this->data_size);
                DEBUG_PRINT_2("with %p, size %d\n", merge_block, merge_block->data_size);
                merge_block->data_size += add_this->data_size + sizeof (block_info_t);
                merge_block->next_block = add_this->next_block;
            }
        }

        // merge_block is now the results of our attempt to merge with
        // the previous block. may be a new, bigger block, or may just be
        // add_this all by itself. In either case,
        // see if we can merge what we have with the next block.
        if (merge_block->next_block != NULL) {
            if ((char *) merge_block->next_block ==
                    ADD_BYTES(BLOCK_DATA(merge_block), merge_block->data_size)) {

                // merge with next block
                DEBUG_PRINT_2("merge next %p, size %d ", merge_block, merge_block->data_size);
                DEBUG_PRINT_2("with %p, size %d\n", merge_block->next_block,
                        merge_block->next_block->data_size);
                merge_block->data_size += merge_block->next_block->data_size + sizeof (block_info_t);
                merge_block->next_block = merge_block->next_block->next_block;
            }
        }
    }
}

/* this function tries to create a new, free block from excess 
 * allocation in the given block allocation, if there is enough of it. if
 * there is, it will shrink size of the given block to just fit
 * its needs, and use the leftover space to create a new block,
 * and add that to the free block list. 
 */
void release_excess(heap_header_t *hdr, block_info_t *block_info, unsigned int block_size) {
    // this only does anything if the block_size really used is small enough
    // that it leaves enough room within data_size to hold it plus a usable new block
    //

    if (block_info->data_size >= block_size + sizeof (block_info_t) + ALIGNMENT) {

        // ok, we have room. the new block_info_t will start at the
        // address following the bytes actually needed by the original block.
        //
        block_info_t *new_block = (block_info_t *) ADD_BYTES(block_info,
                block_size + sizeof (block_info_t));

        // now calculate data size for this new block
        //
        new_block->data_size = block_info->data_size - block_size - sizeof (block_info_t);

        // the original block info is now made smaller--just enough to hold the actual data
        block_info->data_size = block_size;

        // now add the new free block to the list of free blocks
        add_to_free_list(hdr, new_block);
        DEBUG_PRINT_1("release_excess created new free block of %u bytes\n", new_block->data_size);
    }

}

/* this tries to look through the list of free blocks and see if any can be
 * combined into larger free blocks. time consuming, so we only use this if
 * we have free blocks but can't find one big enough for our needs
 
void consolidate_free_blocks(heap_header_t *hdr) {
    //print_debug_heap_state("starting consolidate", hdr);


    //print_debug_heap_state("leaving consolidate", hdr);
}
 */

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First sanity check the inputs. return if obviously crap.
 *
 *
 */
void hl_init(void *heap, unsigned int heap_size) {
    DEBUG_PRINT_2("init_heap--addr %p, size %u, ", heap, heap_size);
    DEBUG_PRINT_2("heaphdr size: %lu, blockhdr size: %lu\n", sizeof(heap_header_t), sizeof(block_info_t));
    DEBUG_PRINT_2("unsigned int size: %lu, blockinfo ptr size: %lu\n", sizeof(unsigned int), sizeof((block_info_t *)));

    // this would be our non-graceful failure for bad input
    if (heap == NULL || heap_size < MIN_HEAP_SIZE) {
        return;
    }

    // so when we start, we have 0 used blocks,
    // and one free block with size of all usable mem
    // this top level struct goes at base of our allocated mem, but
    // make sure it's aligned.
    heap_header_t *hdr = (heap_header_t *) ALIGN_UP(heap);

    hdr->heap_size = heap_size;
    hdr->first_used_block = NULL;

    // right now we have one huge free block. it starts at the
    // first aligned memory location after the heap header, and
    // ends at the last aligned memory block in our heap. our 
    // block info header structs are 8 byte aligned, so we just need to 
    // make sure the first block starts 8 byte aligned, and everything
    // should work out after that. maybe.

    /* need this?
    char *first_free_byte = ADD_BYTES(heap, sizeof (heap_header_t));
    int misaligned_by = ((uintptr_t) first_free_byte) % ALIGNMENT;
    int skip_bytes = (ALIGNMENT - misaligned_by) % ALIGNMENT;
 
    hdr->first_free_block = (block_info_t *) ADD_BYTES(first_free_byte, skip_bytes);
     */

    hdr->first_free_block = (block_info_t *) ADD_BYTES(hdr, sizeof (heap_header_t));
    hdr->first_free_block->next_block = NULL;

    // we're going to make sure all blocks are also aligned at their end,
    // so block merging can work well even after odd-sized allocs. do that with
    // this first block to star.
    int hdr_offset = ((char*) hdr) - ((char*) heap);
    unsigned int max_possible_bytes = heap_size - sizeof (heap_header_t) -
            sizeof (block_info_t) - hdr_offset - ALIGNMENT;
    hdr->first_free_block->data_size = (max_possible_bytes / ALIGNMENT) * ALIGNMENT;

    print_debug_heap_state("End of hl_init", hdr);
    return;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First does sanity check on inputs. return NULL if obviously no good.
 * The finds the "best" fit of free blocks for this block_size request.
 * If there's extra space in the best fit block, it will use that
 * space to create a new free block.
 */
void *hl_alloc(void *heap, unsigned int alloc_size) {
    DEBUG_PRINT_1("in hl_alloc, want %u bytes\n", alloc_size);

    // input sanity checks
    if (heap == NULL || alloc_size < 1) return NULL;

    heap_header_t *hdr = (heap_header_t *) ALIGN_UP(heap);
    print_debug_heap_state("Start of hl_alloc", hdr);

    // no need to screw around if no mem available
    if (hdr->first_free_block == NULL || alloc_size > hdr->heap_size) {
        return NULL;
    }

    // assuming we got decent inputs, first find best free block to
    // allocate. we will only allocate blocks that are multiples of
    // ALIGNMENT bytes. 
    unsigned int block_size = ALIGNED_SIZE(alloc_size);

    // look at all free blocks. keep track of which was best and use it.
    block_info_t *try_this = hdr->first_free_block;
    block_info_t *prev = NULL;
    block_info_t *best_block = NULL;
    block_info_t *best_prev = NULL;

    // for test, try just returning first, not best
#define DO_BEST 1
#if DO_BEST
    unsigned int best_block_size = hdr->heap_size + 1; // bigger than max possible
    while (try_this != NULL) {
        if (try_this->data_size >= block_size && try_this->data_size < best_block_size) {
            best_block = try_this;
            best_block_size = try_this->data_size;
            best_prev = prev;
        }
        prev = try_this;
        try_this = try_this->next_block;
    }
#else
    while (try_this != NULL && best_block == NULL) {
        if (try_this->data_size >= block_size) {
            best_block = try_this;
            best_prev = prev;
        } else {
            prev = try_this;
            try_this = try_this->next_block;
        }
    }
#endif

    // if we did find a block to use, we also know its previous block. remove
    // from list of free blocks and add to list of used blocks
    if (best_block != NULL) {
        DEBUG_PRINT_2("best_block addr: %p, size: %u\n", best_block, best_block->data_size);
        if (best_prev == NULL) {
            // best block was first in list
            hdr->first_free_block = best_block->next_block;
        } else {
            best_prev->next_block = best_block->next_block;
        }

        // just add at top of list of used blocks
        best_block->next_block = hdr->first_used_block;
        hdr->first_used_block = best_block;

        // finally, if there's any extra allocation in this block,
        // free it up as a new block
        release_excess(hdr, best_block, block_size);
        DEBUG_PRINT_3("Successful alloc. Request: %u, alloc'd block: %u at %p\n",
                alloc_size, best_block->data_size, BLOCK_DATA(best_block));
    } else {
        // no block found
        DEBUG_PRINT_1("hl_alloc can't find %d bytes\n", alloc_size);
        print_debug_heap_state("End of hl_alloc--fail", hdr);
        return NULL;
    }

    print_debug_heap_state("End of hl_alloc--success", hdr);

    return (void *) (BLOCK_DATA(best_block));
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * we search the list of used blocks for one whose data ptr
 * matches the input block ptr. If there is none matching, 
 * do nothing. There should be one though, and we move that
 * block from the used list to the free list. Just returns
 * without action if either arg is null.
 */
void hl_release(void *heap, void *block) {
    if (heap == NULL) return;

    heap_header_t *hdr = (heap_header_t *) ALIGN_UP(heap);
    print_debug_heap_state("Start of hl_release", hdr);

    // we only need to do any work if block looks possible
    if (block != NULL && (char*) block < ADD_BYTES(hdr, hdr->heap_size)) {

        // find the indicated block of memory
        block_info_t *check_block = hdr->first_used_block;
        while (check_block != NULL && BLOCK_DATA(check_block) != block) {
            check_block = check_block->next_block;
        }

        if (check_block != NULL) {
            // remove from used list and add to free list
            remove_from_used_list(hdr, block);
            add_to_free_list(hdr, check_block);
        }
    }
    print_debug_heap_state("End of hl_release", hdr);
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_resize(void *heap, void *block, unsigned int new_size) {
    // handle goofball input args first
    if (heap == NULL || new_size < 1)
        return NULL;

    heap_header_t *hdr = (heap_header_t *) ALIGN_UP(heap);
    print_debug_heap_state("Start of hl_resize", hdr);

    if (block == NULL)
        return hl_alloc(heap, new_size);

    // if we can't find legit block info for the block ptr, return
    block_info_t *block_info = get_block_info_for(hdr, block);
    if (block_info == NULL)
        DEBUG_PRINT_1("Resize could not find %p\n", block);
    return NULL;

    // OK, now we probably have legit input values. 
    void *return_this = NULL;
    DEBUG_PRINT_3("Trying to resize %p from %u to %u\n", block_info, block_info->data_size, new_size);

    unsigned int real_new_size = ALIGNED_SIZE(new_size);

    if (block_info->data_size == new_size) {
        return_this = block;
    } else if (real_new_size > block_info->data_size) {
        // so we need a bigger block. if we can allocate one, we
        // copy the current data to the new block, free this one,
        // and return the new ptr. if we can't alloc a new block,
        // we return null.
        void *new_block = hl_alloc(heap, real_new_size);
        if (new_block != NULL) {
            memcpy(new_block, block, block_info->data_size);
            hl_release(heap, block);
        }
        return_this = new_block;
    } else {
        // if we're here, new_size is less than current size.
        // we will try to create a new free block from the
        // now unused portion of the original allocation,
        // if the reduction was enough to allow that
        release_excess(hdr, block_info, real_new_size);
        return_this = block;
    }

    print_debug_heap_state("End of hl_resize", hdr);
    return return_this;
}

