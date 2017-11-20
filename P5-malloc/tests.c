#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"

#define HEAP_SIZE 1024
#define NUM_TESTS 24
#define NPOINTERS 100

// TODO: Add test descriptions as you add more tests...
const char* test_descriptions[] = {
    /* our SPEC tests */
    /* 0 */ "single init, should return without error",
    /* 1 */ "single init then single alloc, should pass",
    /* 2 */ "single alloc which should fail b/c heap is not big enough",
    /* 3 */ "multiple allocs, verifying no hard-coded block limit",
    /* your SPEC tests */
    /* 4  */ "resize with null block pointer should act as alloc",
    /* 5  */ "release with null block pointer should act as NOP",
    /* 6  */ "alloc return address 8-byte aligned",
    /* 7  */ "resize change location of block when not possible to increase the size of the current block but there is room elsewhere",
    /* 8  */ "When resizing to smaller size, contents inside are the same",
    /* 9  */ "When resize changes location, contents should be preserved",
    /* 10 */ "your description here",
    /* 11 */ "your description here",
    /* 12 */ "your description here",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "your description here",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "your description here",
    /* 18 */ "your description here",
    /* 19 */ "your description here",
    /* 20 */ "your description here",
    /* 21 */ "your description here",
    /* 22 */ "checks to see if an alloc will return a block of memory that runs past end of heap",
    /* 23 */ "alloc following release should not reuse released memory unless big enough to fit requested bytes",
};

/* -------------------- PRINT DEBUG FNS ----------------- */

/* macros for debug message printing. uses fmt string to print out other values
 */
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


/* ------------------ COMPLETED SPEC TESTS ------------------------- */

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init
 * SPECIFICATION BEING TESTED:
 * hl_init should successfully complete (without producing a seg
 * fault) for a HEAP_SIZE of 1024 or more.
 *
 * MANIFESTATION OF ERROR:
 * A basic test of hl_init.  If hl_init has an eggregious programming
 * error, this simple call would detect the problem for you by
 * crashing.
 *
 * Note: this shows you how to create a test that should succeed.
 */
int test00() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    return SUCCESS;
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is room in the heap for a request for a block, hl_alloc
 * should sucessfully return a pointer to the requested block.
 *
 * MANIFESTATION OF ERROR:
 * The call to hl_alloc will return NULL if the library cannot find a
 * block for the user (even though the test is set up such that the
 * library should be able to succeed).
 */
int test01() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns null, test21 returns FAILURE (==0)
    return (hl_alloc(heap, HEAP_SIZE/2) != NULL);
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is not enough room in the heap for a request for a block,
 * hl_alloc should return NULL.
 *
 * MANIFESTATION OF ERROR:
 * This test is designed to request a block that is definitely too big
 * for the library to find. If hl_alloc returns a pointer, the library is flawed.
 *
 * Notice that heaplame's hl_alloc does NOT return NULL. (This is one
 * of many bugs in heaplame.)
 *
 * Note: this shows you how to create a test that should fail.
 *
 * Surely it would be a good idea to test this SPEC with more than
 * just 1 call to alloc, no? 
 */
int test02() {

    // simulated heap is just an array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns NULL, test22 returns SUCCESS (==1)
    return !hl_alloc(heap, HEAP_SIZE*2);

}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * There should be no hard-coded limit to the number of blocks heaplib
 * can support. So if the heap gets larger, so should the number of
 * supported blocks.
 *
 * MANIFESTATION OF ERROR:
 * See how many blocks are supported with heap size N. This number should
 * increase with heap size 2N. Otherwise fail!
 *
 * Note: unless it is fundamentally changed, heaplame will always fail
 * this test. That is fine. The test will have more meaning when run on
 * your heaplib.c
 */
int test03() {

    // now we simulate 2 heaps, once 2x size of the first
    char heap[HEAP_SIZE], heap2[HEAP_SIZE*2];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, HEAP_SIZE);

    while(true) {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, HEAP_SIZE*2);

    while(true) {
        int *array = hl_alloc(heap2, 8);
        if (array)
            num_blocks2++;
        else
            break;
    }

    DEBUG_PRINT_2("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);

    // hoping to return SUCCESS (==1)
    return (num_blocks2 > num_blocks);
}

/* ------------------ YOUR SPEC TESTS ------------------------- */

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: When a null pointer is passed into the block pointer,
 * resize should work as hl_alloc
 *
 * MANIFESTATION OF ERROR:
 * See if hl_resize acts as hl_alloc when block pointer is NULL. If hl_resize returns a NULL pointer,
 * it should fail. 
 */
int test04() {
    
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    return  (hl_resize(heap, NULL, HEAP_SIZE/2) != NULL);
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: release
 * SPECIFICATION BEING TESTED: 
 * When release is called with a null block-pointer, it should act as a NOP
 *
 * MANIFESTATION OF ERROR:
 * 
 */
int test05() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    hl_release(heap, NULL);

    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test06() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    int *array = hl_alloc(heap, HEAP_SIZE/2);
    
    return (((uintptr_t) array) % 8 == 0);
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_resize()
 * SPECIFICATION BEING TESTED: hl_resize() should change the location of a
 * block if it is not possible to resize at current location, but there is
 * room elsewhere.
 *
 *
 * MANIFESTATION OF ERROR:
 * 1. address of new block has not changed
 *
 */
int test07() {
	
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    char *array = hl_alloc(heap, HEAP_SIZE/4);
	array[0] = '1';
	
    char *newarray = hl_alloc(heap, HEAP_SIZE/4);
	newarray[0] = '2';
    
	DEBUG_PRINT_1("array: %p\n", array);
	DEBUG_PRINT_1("newarray: %p\n", newarray);

	// this resize should force the block to a new location past newarray
    char *resizearray = hl_resize(heap, array, HEAP_SIZE/3);
    
	DEBUG_PRINT_1("resizearray: %p\n", resizearray);
    
    return (array != resizearray);
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test08() {
     
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    char *array = (char *) hl_alloc(heap, HEAP_SIZE/4);

    for (int i =0; i< HEAP_SIZE/4; i++) {
	array[i] = 'A';
    }

    char *newarray = (char *) hl_resize(heap, array, HEAP_SIZE/8);
    
    bool testfail = SUCCESS;

    for (int j=0; j< HEAP_SIZE/8; j++) {
		if (array[j] != newarray[j]) {
			testfail = NULL;
		}
    }

    return testfail;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test09() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    char *arrayA =(char*) hl_alloc(heap, HEAP_SIZE/8);

    for(int i=0; i< (HEAP_SIZE/8)/sizeof(char) ; i++) {
	arrayA[i]= 'A';
		if (i == 0) {
			DEBUG_PRINT_1("address of arrayA[0] = %p\n", &arrayA[0]);
			DEBUG_PRINT_1("value of arrayA[0] = %c\n ", arrayA[0]);
        }
	DEBUG_PRINT_2("%d : %c \n", i, arrayA[i]);
    }

    char *arrayB = (char*) hl_alloc(heap, HEAP_SIZE/8);

    for (int j=0; j< (HEAP_SIZE/8)/sizeof(char); j++) {
		arrayB[j] = 'B';
		DEBUG_PRINT_2("%d : %c \n", j, arrayA[j]);
    }

    //for (int t=0; t < HEAP_SIZE; t++) {
	//DEBUG_PRINT_2("%d: %c \n", t, heap[t]);
    //}

    DEBUG_PRINT_1("pointer for arrayA before resize: %p \n", arrayA);
    DEBUG_PRINT_1("before resize: arrayA[0] = %c \n", arrayA[0]);
    DEBUG_PRINT_1("before resize arrayA[0] address = %p \n", &arrayA[0]);
    DEBUG_PRINT_1("pointer for arrayB before resize: %p \n", arrayB);

    char *resizeArray = (char *) hl_resize(heap, arrayA, HEAP_SIZE/4);
    
    DEBUG_PRINT_1("pointer for arrayA: %p \n", arrayA);
    DEBUG_PRINT_1("arrayA[0] = %c \n", arrayA[0]);
    DEBUG_PRINT_1("pointer for arrayB: %p \n", arrayB);
    DEBUG_PRINT_1("pointer for resizeArray:  %p \n", resizeArray);
    DEBUG_PRINT_1("resizeArray[0] = %c \n", resizeArray[0]);
    
    bool testfail = SUCCESS;

    //for (int u =0; u < HEAP_SIZE; u++) {
	//	DEBUG_PRINT_2("%d: %c \n", u, heap[u]);
    //}
    
    //resizeArray[0] = arrayA[0];
    
    for (int k=0; k < (HEAP_SIZE/8)/sizeof(char); k++) {
		DEBUG_PRINT_3("address %p: %d : %c \n", &resizeArray[k], k, resizeArray[k]);
		if (resizeArray[k] != 'A'){
			testfail = NULL;
			DEBUG_PRINT_1("testfail changed to %d \n", testfail);
		}
     }
    
    //for (int u=0; u<HEAP_SIZE; u++) {
	//DEBUG_PRINT_2("%d: %c \n", u, heap[u]);
    //}

    DEBUG_PRINT_1("testfail =  %d \n", testfail);
    
    return testfail;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: 
 * SPECIFICATION BEING TESTED: 
 *
 * MANIFESTATION OF ERROR: 
 */
int test10() {
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test11() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test12() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test13() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test14() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test15() {

    return FAILURE;
}

/* ------------------ STRESS TESTS ------------------------- */

/* THIS TEST IS NOT FINISHED. It is a stress test, but it does not
 * actually test to see whether the library or the user writes 
 * past the heap. You are encouraged to complete this test.
 * 
 * FUNCTIONS BEING TESTED: alloc, free
 * SPECIFICATION BEING TESTED:
 * The library should not give user "permission" to write off the end
 * of the heap. Nor should the library ever write off the end of the heap.
 *
 * MANIFESTATION OF ERROR:
 * If we track the data on each end of the heap, it should never be
 * written to by the library or a good user.
 *
 */
int test16() {
    int n_tries    = 10000;
    int block_size = 16;

    // 1024 bytes of padding--fill it with Z's, check at end for change
    // --------------------
    // 1024 bytes of "heap"--fill all blocks allocated with x's
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"--fill it with A's, check at end for change
    char memarea[HEAP_SIZE*3];

    char *heap_start = &memarea[HEAP_SIZE]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];
	
	// fill padding areas above and below heap with values we can 
	// check at the end to see if any were overwritten
	memset(memarea, 'A', HEAP_SIZE);
	memset(memarea + HEAP_SIZE*2, 'B', HEAP_SIZE);

    // zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));

    hl_init(heap_start, HEAP_SIZE);
    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0) {
            pointers[index] = hl_alloc(heap_start,  block_size);
			// if you got a block allocated, fill it with x's
			if (pointers[index] != NULL) {
				memset(pointers[index], 'x', block_size);
			}
        }
        else{
            hl_release(heap_start, pointers[index]);
            pointers[index] = 0;
        }
    }
	
	// OK after all that allocating, releasing and writing, check
	// to see if we ever wrote beyond the top or bottom of the memory
	// area allocated for the heap.
	bool overwrite_below = 0;
	bool overwrite_above = 0;
	
	// check area below the heap
	for (int i = 0; i < HEAP_SIZE && overwrite_below == 0; i++) {
		overwrite_below = (memarea[i] != 'A');
		if (overwrite_below) {
			DEBUG_PRINT_3("Memory below heap overwritten. HeapStart: %p, Written Addr: %p, Written Val: %c\n", heap_start, &memarea[i], memarea[i]);
		}	
	}
	
	// check area above the heap
	for (int i = HEAP_SIZE*2; i < HEAP_SIZE*3 && overwrite_above == 0; i++) {
		overwrite_above = (memarea[i] != 'Z');
		if (overwrite_above) {
			DEBUG_PRINT_3("Memory above heap overwritten. HeapStart: %p, Written Addr: %p, Written Val: %c\n", heap_start, &memarea[i], memarea[i]);
		}
	}
	
    return (!overwrite_above && !overwrite_below);
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: 
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test17() {

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test18() {

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test19() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test20() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test21() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: hl_alloc
 * INTEGRITY OR DATA CORRUPTION? Data corruption
 *
 * MANIFESTATION OF ERROR: we attempt to allocate more memory than
 * heap has space for. if the third alloc gets a non-null result,
 * the heap manager has allocated a block of memory that will
 * go beyond the end of the heap.
 *
 */
int test22() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
	bool result = FAILURE;
	
	// we'll use a block size that we should be able to safely
	// allocate 2 of, but not 3
	int blocksize = HEAP_SIZE/2 - 128;
	
	// array of memory blocks we'll allocate and release
	char *blocks[3];
	
	// first two should get valid pointers. third should not, because
	// there is not enough room
	blocks[0] = hl_alloc(heap, blocksize);
	blocks[1] = hl_alloc(heap, blocksize);
	blocks[2] = hl_alloc(heap, blocksize);
	
	DEBUG_PRINT_3("block0: %p, block1: %p, block2: %p\n", blocks[0], blocks[1], blocks[2]);
	
	if (blocks[2] != NULL) {
		DEBUG_PRINT_2("End of Heap: %p, End of Block2: %p\n", &heap[0] + HEAP_SIZE, blocks[2] + blocksize);
	}
	
	result = (blocks[2] == NULL) || (blocks[2] + blocksize <= &heap[0] + HEAP_SIZE);
	
    return result;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: hl_alloc, hl_release
 * INTEGRITY OR DATA CORRUPTION? Data corruption
 *
 * MANIFESTATION OF ERROR: 2 blocks are allocated. the first is released.
 * another bigger block is allocated. it should not overlap with the 2nd block allocated above.
 *
 */
int test23() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
	
	// array of memory blocks we'll allocate and release
	char *blocks[3];
	
	// allocate some memory for 3 blocks
	blocks[0] = hl_alloc(heap, 128);
	blocks[1] = hl_alloc(heap, 128);
	hl_release(heap, blocks[0]);
	blocks[2] = hl_alloc(heap, 144);
	
	// we have a problem if hl_alloc put blocks[2] in the space
	// it released for block[0], since there is not enough room
	// there.

	DEBUG_PRINT_3("block0: %p, block1: %p, block2: %p \n", blocks[0], blocks[1], blocks[2]);
	
	bool result = (blocks[2] > blocks[1]);

    return result;
}
