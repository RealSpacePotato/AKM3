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
    /* 7  */ "alloc, release, alloc should not return null pointer",
    /* 8  */ "When resizing to smaller size, contents inside are the same",
    /* 9  */ "When resize changes location, contents should be preserved",
    /* 10 */ "resize return address 8-byte aligned",
    /* 11 */ "init with heap size too small. should exit(1)",
    /* 12 */ "init with null heap pointer, should exit(1)",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "your description here",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "Structural Integrity - alloc and resize",
    /* 18 */ "Structural Integrity - alloc and release",
    /* 19 */ "Data Integrity - data preservation - alloc and release",
    /* 20 */ "Data Integrity - data preservation - alloc and resize",
    /* 21 */ "your description here",
    /* 22 */ "your description here",
    /* 23 */ "your description here",
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
#ifdef PRINT_DEBUG
    printf("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);
#endif

    // hoping to return SUCCESS (==1)
    return (num_blocks2 > num_blocks);
}

/* ------------------ YOUR SPEC TESTS ------------------------- */

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: When a null pointer is passed into the block pointer,
 * resize should work as hl_alloc.
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
 * This test is designed to see with release with a null pointer acts as a NOP. If hl_release releases
 * some block in memory, the library is flawed. 
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
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * The return address of hl_alloc is 8-byte aligned.
 *
 * MANIFESTATION OF ERROR:
 * If the return address of hl_alloc is not 8-byte aligned, the library is flawed.
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
 * FUNCTIONS BEING TESTED: alloc, release
 * SPECIFICATION BEING TESTED:
 * hl_alloc after a release should not return a null pointer.
 *
 * MANIFESTATION OF ERROR:
 * If hl_alloc returns a null pointer after a release, the library is flawed.
 */
int test07() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    char *array = hl_alloc(heap, (3*HEAP_SIZE)/4);
    hl_release(heap, array);

    return (hl_alloc(heap, (3*HEAP_SIZE)/4) != NULL);

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc, resize
 * SPECIFICATION BEING TESTED:
 * hl_resize should not change the contents of the array returned by hl_alloc
 *
 * MANIFESTATION OF ERROR:
 * If the contents differ for arrays returned by hl_resize and hl_alloc, the library is flawed.
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
 * FUNCTIONS BEING TESTED: alloc, resize
 * SPECIFICATION BEING TESTED:
 * When resizes changes location of pointer in heap, the contents should be preserved.
 *
 * MANIFESTATION OF ERROR:
 * If the contents differ after resize is called, the library is flawed. 
 */
int test09() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    char *arrayA =(char*) hl_alloc(heap, HEAP_SIZE/8);

    for(int i=0; i< (HEAP_SIZE/8)/sizeof(char) ; i++) {
	arrayA[i]= 'A';
    }

    char *arrayB = (char*) hl_alloc(heap, HEAP_SIZE/8);

    for (int j=0; j< (HEAP_SIZE/8)/sizeof(char); j++) {
	arrayB[j] = 'B';
    }

    char *resizeArray = (char *) hl_resize(heap, arrayA, HEAP_SIZE/4);

    bool testfail = SUCCESS;

    for (int k=0; k < (HEAP_SIZE/8)/sizeof(char); k++) {
	if (resizeArray[k] != 'A'){
	  testfail = NULL;
	}
     }

    return testfail;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED:
 * The return address of resize should be 8-byte aligned.
 *
 * MANIFESTATION OF ERROR:
 * If the return address of resize is not 8-byte aligned, the library is flawed.
 */
int test10() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    int *array = hl_alloc(heap,HEAP_SIZE/2);
    
    int *newarray = hl_resize(heap, array, (3*HEAP_SIZE)/4);

    return (((uintptr_t) newarray) % 8 == 0);
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_init()
 * SPECIFICATION BEING TESTED: exits if heap size lower than min
 *
 *
 * MANIFESTATION OF ERROR: program should exit
 *
 */
int test11() {
    char heap[MIN_HEAP_SIZE-1];
    hl_init(heap, MIN_HEAP_SIZE-1);


     return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_init
 * SPECIFICATION BEING TESTED: non-graceful fail if heap* is NULL
 *
 *
 * MANIFESTATION OF ERROR: should exit(1)
 *
 */
int test12() {
    char *bad_heap_arg = NULL;

    hl_init(bad_heap_arg, MIN_HEAP_SIZE);

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
    int block_size = 220;

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
	memset(memarea + HEAP_SIZE*2, 'Z', HEAP_SIZE);

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
 * FUNCTIONS BEING TESTED: alloc and resize
 * INTEGRITY OR DATA CORRUPTION? STRUCTURAL INTEGRITY
 * 
 * MANIFESTATION OF ERROR:
 * At some point a field in the heap thought to contain a pointer will no longer contain a valid address.
 */
int test17() {

    int n_tries = 10000;

    char heap[HEAP_SIZE*3];

    char *pointers[NPOINTERS];

    //zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));

    hl_init(heap, HEAP_SIZE*3);

    srandom(0);
    for (int i =0; i < n_tries; i++) {
	int index = random() % NPOINTERS;
	if (pointers[index] == 0) {
	     //NULL pointer, set pointer to heap
	     pointers[index] = hl_alloc(heap, random() % (HEAP_SIZE*2));
	} else {
	    //resize the size of heap
	    pointers[index] = hl_resize(heap, pointers[index],random() % (HEAP_SIZE*2));
	}
    }

    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc, release
 * INTEGRITY OR DATA CORRUPTION? STRUCTURAL INTEGRITY
 *
 * MANIFESTATION OF ERROR:
 * At some point a field in the heap thought to contain a pointer will no longer contain a valid address.
 */
int test18() {

    int n_tries = 10000;

    char heap[HEAP_SIZE*3];
    char *pointers[NPOINTERS];

    memset(pointers, 0, NPOINTERS*sizeof(char *));

    hl_init(heap, HEAP_SIZE*3);

    srandom(0);
    for (int i =0; i < n_tries; i++) {
	int index = random() % NPOINTERS;
	if (pointers[index] == 0) {
	    pointers[index] = hl_alloc(heap, random() % (HEAP_SIZE*2));
	} else {
	    hl_release(heap, pointers[index]);
	    pointers[index] = 0;
	}
    }

    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc , release
 * INTEGRITY OR DATA CORRUPTION? Data Integrity
 *
 * MANIFESTATION OF ERROR:
 * The library itself writes something into the block portion of the heap, which it should never do 
 * unless that block has been freed by the user
 */
int test19() {

    int n_tries = 10000;
    int block_size = 40;

    char heap[HEAP_SIZE*3];
    char *pointers[NPOINTERS];

    //zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));

    hl_init(heap, HEAP_SIZE*3);

    char *array = (char *) hl_alloc(heap, HEAP_SIZE);
    //set everything in array to A
    for (int i=0; i<HEAP_SIZE; i++) {
	array[i] = 'A';
    }

    //set pointers[0] to array
    pointers[0] = array;

    srandom(0);
    for (int j=0; j<n_tries; j++) {
	int index = random() % NPOINTERS;
	if (index != 0) {
	    if (pointers[index] == 0) {
		pointers[index] = hl_alloc(heap, block_size);
		//change the contents in pointers[index] to B
		for (int t= 0; t<block_size && (pointers[index] != NULL); t++) {
		    pointers[index][t] = 'B';
		}
	    } else {
		hl_release(heap, pointers[index]);
		pointers[index] = 0;
	    }
	}
    }

    bool testresult = SUCCESS;

    //check if everything in pointers[0] is A
    for (int k=0; k<HEAP_SIZE; k++) {
	if(pointers[0][k] != 'A') {
	    testresult = FAILURE;
	}
    }

    return testresult;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc and resize
 * INTEGRITY OR DATA CORRUPTION? Data Integrity
 *
 * MANIFESTATION OF ERROR:
 * The library itself writes something into the block portion of the heap, which it should never do
 * unless that block has been freed by the user
 */
int test20() {

    int n_tries = 10000;
    int block_size = 40;

    char heap[HEAP_SIZE*3];
    char *pointers[NPOINTERS];

    //zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));
    hl_init(heap, HEAP_SIZE*3);
    char *array = (char *) hl_alloc(heap, HEAP_SIZE);

    //set everything in array to A
    for (int i =0; i<HEAP_SIZE; i++) {
	array[i] = 'A';
    }

    //set pointers[0] to array
    pointers[0] = array;

    srandom(0);
    for (int j=0; j<n_tries; j++) {
	int index = random() % NPOINTERS;
	if (index !=0) {
	    if(pointers[index] == NULL) {
		pointers[index] = hl_alloc(heap, block_size);
		if (pointers[index] != NULL) {
		    memset(pointers[index], 'B', block_size);
		}
	    } else {
		int number = random() % (block_size*2);
		pointers[index] = hl_resize(heap, pointers[index], number);
		if (pointers[index] != NULL) {
			memset(pointers[index], 'C', number);
		}
	    }
	}
    }

    bool testresult = SUCCESS;

    //check if everything in pointers[0] is A
    for (int k = 0; k < HEAP_SIZE; k++) {
	if(pointers[0][k] != 'A') {
	    testresult = FAILURE;
        }
    }

    return testresult;
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
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test22() {

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
int test23() {

     return FAILURE;
}
