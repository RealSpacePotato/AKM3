#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// -----------DEFINITIONS-----------------

// header info for each block of memory we're managing
// we'll be using these in doubly linked lists
typedef struct _block_info_t {
    unsigned int data_size;   // amount of user data--doesn't include block info
    struct _block_info_t *prev_block;
    struct _block_info_t *next_block;
    char *data; // this is the pointer to the user data
} block_info_t;

// main struct for top level mem manager info
typedef struct _heap_header_t {
    unsigned int heap_size;
    block_info_t *first_free_block;
    block_info_t *first_used_block;
} heap_header_t ;

/* Useful shorthand: casts a pointer to a (char *) before adding/subtracting */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))
#define SUB_BYTES(base_addr, num_bytes) (((char *)(base_addr)) - (num_bytes))

/* Given a target address, returns the next properly aligned address. same if
 * input address is already aligned.
 */
#define ALIGN_UP(addr) ADD_BYTES(addr, ((ALIGNMENT - (((uintptr_t)addr) % ALIGNMENT)) % ALIGNMENT))

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
    if (((char *)block) < ADD_BYTES(hdr, sizeof(heap_header_t)) ||
            ((char *)block) >= ADD_BYTES(hdr, hdr->heap_size)) {
        return NULL;
    }
    
    // OK, might be legit. For now just do fast, easy way.
    // Will cause violent death if ptr is bogus though.
    // Maybe be more careful by doing a search?
    return (block_info_t*) SUB_BYTES(block, sizeof(block_info_t));
}

// given a requested block size, returns pointer to the "best"
// free block to use for this request.
//
block_info_t *find_best_free_block(heap_header_t *hdr, unsigned int desired_block_size){
    
    DEBUG_PRINT_1("In find_best_free_block, size = %u\n", desired_block_size);

    // stupid for now. just returns first free block that fits
    block_info_t *try_this = hdr->first_free_block;
    block_info_t *best_block = NULL;

    while (try_this != NULL && best_block == NULL) {
        if (try_this->data_size >= desired_block_size) {
            best_block = try_this;
        } else {
            try_this = try_this->next_block;
        }
    }

    return best_block;
}

/* this removes the given free block from the list of
 * free blocks in hdr. does nothing if either arg is null.
 */
void remove_from_free_list(heap_header_t *hdr, block_info_t *remove_this) {
 
    // do nothing for sketchy inputs
    if (hdr == NULL || remove_this == NULL) return;
    
// if it's the head of the list, its next block is now the head
    if (remove_this == hdr->first_free_block) {
        hdr->first_free_block = remove_this->next_block;
        if (hdr->first_free_block != NULL){
            hdr->first_free_block->prev_block = NULL;
        }
    } else {
        // normal case is to adjust prev and next blocks to 
        // point directly at each other, skipping over remove_this
        if (remove_this->prev_block != NULL) {
            remove_this->prev_block->next_block = remove_this->next_block;
        }
       if (remove_this->next_block != NULL) {
            remove_this->next_block->prev_block = remove_this->prev_block;
        }
    }
}

/* this removes the given free block from the list of
 * used blocks in hdr. does nothing if either arg is null
 */
void remove_from_used_list(heap_header_t *hdr, block_info_t *remove_this) {
    
    // do nothing for sketchy inputs
    if (hdr == NULL || remove_this == NULL) return;
    
    // if it's the head of the list, its next block is now the head
    if (remove_this == hdr->first_used_block) {
        hdr->first_used_block = remove_this->next_block;
        if (hdr->first_used_block != NULL) {
            hdr->first_used_block->prev_block = NULL;
        }
    } else {
        // normal case is to adjust prev and next blocks to 
        // point directly at each other, skipping over remove_this
        if (remove_this->prev_block != NULL) {
            remove_this->prev_block->next_block = remove_this->next_block;
        }
       if (remove_this->next_block != NULL) {
            remove_this->next_block->prev_block = remove_this->prev_block;
        }
    }
}
/* this adds the given free block to the list of
 * free blocks in hdr. 
 * maintains list in lo->hi address order. this will
 * be useful when we need to consolidate free blocks
 * to make bigger blocks available again
 */
void add_to_free_list(heap_header_t *hdr, block_info_t *add_this) {
    // let's do super easy case first--the list is empty
    block_info_t *check_this = hdr->first_free_block;
    if (check_this == NULL) {
        hdr->first_free_block = add_this;
        add_this->prev_block = NULL;
        add_this->next_block = NULL;
    } else {
        // ok, there's at least one block in the free list.
        // walk down the list until we find a block with
        // a higher address than add_this, or get to end. 
        // insert there.
        while (check_this->next_block != NULL && check_this < add_this) {
            check_this = check_this->next_block;
        }
        // when we get here, check_this is the last in the 
        // chain and/or check_list is at a higher addr than add_this
        if (check_this < add_this) {
            // insert add_this after check_this
            add_this->prev_block = check_this;
            add_this->next_block = check_this->next_block;
            check_this->next_block = add_this;
            if (add_this->next_block != NULL) {
                add_this->next_block->prev_block = add_this;
            }
        } else {
            // insert add_this before check_this
            add_this->next_block = check_this;
            add_this->prev_block = check_this->prev_block;
            check_this->prev_block = add_this;
            if (add_this->prev_block == NULL) {
                // it's the new head of the list
                hdr->first_free_block = add_this;
            } else {
                // if there was a previous block, set its next to add_this
                add_this->prev_block->next_block = add_this;
            }
        }
    }

    /* old simpler version commented out
    // pretty straightforward since we're adding at head of list
    add_this->prev_block = NULL; // head of the list duh
    add_this->next_block = hdr->first_free_block;
    if (add_this->next_block != NULL) {
        add_this->next_block->prev_block = add_this;
    }
    hdr->first_free_block = add_this;
    */
}

/* this adds the given used block to the list of
 * used blocks in hdr. currently just adds at head.
 * might do something clever later...?
 */
void add_to_used_list(heap_header_t *hdr, block_info_t *add_this) {
    // pretty straightforward since we're adding at head of list
    add_this->prev_block = NULL; // head of the list duh
    add_this->next_block = hdr->first_used_block;
     if (add_this->next_block != NULL) {
        add_this->next_block->prev_block = add_this;
    }   
    hdr->first_used_block = add_this;
}

/* this function tries to create a new, free block from excess 
 * allocation in the given block, if there is enough of it. if
 * there is, it will shrink size of the given block to just fit
 * its needs, and use the leftover space to create a new block,
 * and add that to the free block list. 
 */
void release_excess(heap_header_t *hdr, block_info_t *block_info, unsigned int block_size) {
    // this only does anything if the block_size really used is small enough
    // that it leaves enough room within data_size to hold it, and hold a new block
    if (block_info->data_size > (block_size + sizeof (block_info_t) + ALIGNMENT)) {
        // ok, we have room. the new block_info will start at the next 8-byte aligned
        // address following the bytes actually used by the original block.
        block_info_t *new_block = (block_info_t *) ALIGN_UP(ADD_BYTES(block_info->data, block_size));

        // now set up this new block info to use that excess data
        new_block->data = ADD_BYTES(new_block, sizeof (block_info_t));
        //new_block->data_size = block_info->data_size - block_size - sizeof (block_info_t);
        new_block->data_size = ADD_BYTES(block_info->data, block_info->data_size) - new_block->data;

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
 */
void consolidate_free_blocks(heap_header_t *hdr) {
    print_debug_heap_state("starting consolidate", hdr);
    
    
    print_debug_heap_state("leaving consolidate", hdr);
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First sanity check the inputs. Exit(1) if obviously crap.
 *
 *
 */
void hl_init(void *heap, unsigned int heap_size) {
    DEBUG_PRINT_2("init_heap--heap hdr size: %lu, block hdr size: %lu\n", sizeof(heap_header_t), sizeof(block_info_t));
    
    // this would be our non-graceful failure for bad input
    if (heap == NULL || heap_size < MIN_HEAP_SIZE) {
        exit(1);
    }

    // so when we start, we have 0 used blocks,
    // and one free block with size of all available mem
    // this top level struct goes at base of our allocated mem
    heap_header_t *hdr = (heap_header_t *) heap;

    hdr->heap_size = heap_size;
    hdr->first_used_block = NULL;

    // right now we have one huge free block. it starts at the
    // first aligned memory location after the heap header. our 
    // block info headers are 8 byte aligned, so we just need to 
    // make sure the first block starts 8 byte aligned, and everything
    // should work out after that. maybe.
    char *first_free_byte = ADD_BYTES(heap, sizeof (heap_header_t));
    int misaligned_by = ((uintptr_t) first_free_byte) % ALIGNMENT;
    int skip_bytes = (ALIGNMENT - misaligned_by) % ALIGNMENT;
    
    hdr->first_free_block = (block_info_t *) ADD_BYTES(first_free_byte, skip_bytes);

    hdr->first_free_block->data_size = heap_size - sizeof (heap_header_t) - skip_bytes - sizeof (block_info_t);
    hdr->first_free_block->prev_block = NULL;
    hdr->first_free_block->next_block = NULL;

    // data is the pointer to the available user data. starts just after block info
    hdr->first_free_block->data = ADD_BYTES(hdr->first_free_block, sizeof (block_info_t));

    print_debug_heap_state("End of hl_init", hdr);
    return;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First does sanity check on inputs. Exit(2) if obviously no good.
 * The finds the "best" fit of free blocks for this block_size request.
 * If there's extra space in the best fit block, it will use that
 * space to create a new free block.
 */
void *hl_alloc(void *heap, unsigned int block_size) {

    // input sanity checks
    if (heap == NULL) exit(2);
    
    heap_header_t *hdr = (heap_header_t *) heap;
    print_debug_heap_state("Start of hl_alloc", hdr);
     
    // no need to screw around if no mem available or size is 0
    if (hdr->first_free_block == NULL || block_size < 1) {
        return NULL;
    }

    // assuming we got decent inputs, first find best free block to
    // allocate.
    block_info_t *best_block = find_best_free_block(hdr, block_size);
    
    // if no block works, consolidate free blocks and try again
    if (best_block == NULL) {
        DEBUG_PRINT_1("first try can't find %d bytes\n", block_size);
        consolidate_free_blocks(hdr);
        best_block = find_best_free_block(hdr, block_size);
        
        //if we still couldn't find an acceptable block, give up
        if (best_block == NULL) {
            DEBUG_PRINT_1("hl_alloc can't find %d bytes\n", block_size);
            return NULL;
        }
 
    // if we found a good block, we have a few things to do.
    // we'll need to remove that block from the list of free blocks
    // and add it to the list of used blocks. also we'll see
    // if there is enough leftover space in that best_block to
    // create a new free block with.
    } else {
        DEBUG_PRINT_2("best_block addr: %p, size: %u\n", best_block, best_block->data_size);

        // so now we've found the best block. if it is big enough to hold
        // what we need AND still has room for another usable block,
        // split off that usable portion as a new, free block
        release_excess(hdr, best_block, block_size);

        // regardless of whether we could create a new block with leftover
        // space, now remove best_block from free_list and add to used_list
        remove_from_free_list(hdr, best_block);
        add_to_used_list(hdr, best_block);
    }
    DEBUG_PRINT_1("first used block: %p\n", hdr->first_used_block);
    DEBUG_PRINT_1("first free block: %p\n", hdr->first_free_block);
    print_debug_heap_state("End of hl_alloc", hdr);

    return best_block->data;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * we search the list of used blocks for one whose data ptr
 * matches the input block ptr. If there is none matching, 
 * we exit(3). There should be one though, and we move that
 * block from the used list to the free list. Just returns
 * without action if either arg is null.
 */
void hl_release(void *heap, void *block) {
    if (heap == NULL) return;

    heap_header_t *hdr = (heap_header_t *) heap;
    print_debug_heap_state("Start of hl_release", hdr);

    // we only need to do any work if block != null
    if (block != NULL) {

        // find the indicated block of memory
        block_info_t *check_block = hdr->first_used_block;
        while (check_block != NULL && check_block->data != block) {
            check_block = check_block->next_block;
        }

        if (check_block == NULL) {
            exit(3);
        } else {
            // remove from used list and add to free list
            remove_from_used_list(hdr, check_block);
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
    if (heap == NULL) exit(4);
    
    heap_header_t *hdr = (heap_header_t *) heap;
    print_debug_heap_state("Start of hl_resize", hdr);

    if (new_size < 1) return NULL;
    if (block == NULL) return hl_alloc(heap, new_size);
    
    // if we can't find legit block info for the block ptr, exit(5)
    block_info_t *block_info = get_block_info_for(hdr, block);
    if (block_info == NULL) exit(5);
    
    // OK, now we probably have legit input values. 
    void *return_this = NULL;
    
    if (block_info->data_size == new_size) {
        return_this = block;
    } else if (new_size > block_info->data_size) {
        // so we need a bigger block. if we can allocate one, we
        // copy the current data to the new block, free this one,
        // and return the new ptr. if we can't alloc a new block,
        // we return null.
        void *new_block = hl_alloc(heap, new_size);
        if (new_block != NULL) {
            memcpy(new_block, block, block_info->data_size);
        }
        hl_release(heap, block);
        return_this = new_block;
    } else {
        // if we're here, new_size is less than current size.
        // we will try to create a new free block from the
        // now unused portion of the original allocation,
        // if the reduction was enough to allow that
        release_excess(hdr, block_info, new_size);
        return_this = block;
    }
    
    print_debug_heap_state("End of hl_resize", hdr);
    return return_this;
}

