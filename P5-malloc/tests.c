#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define HEAP_SIZE 3999
#define NUM_TESTS 24
#define NPOINTERS 400

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
    /* 13 */ "check that release consolidates free memory",
    /* 14 */ "single alloc max possible size, should succeed",
    /* 15 */ "your description here",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "Structural Integrity - alloc and resize",
    /* 18 */ "Structural Integrity - alloc and release",
    /* 19 */ "Data Integrity - data preservation - alloc and release",
    /* 20 */ "Data Integrity - data preservation - alloc and resize",
    /* 21 */ "your description here",
    /* 22 */ "your description here",
    /* 23 */ "lots of everything",
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
    return (hl_alloc(heap, HEAP_SIZE / 2) != NULL);
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
    return !hl_alloc(heap, HEAP_SIZE * 2);

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
    char heap[HEAP_SIZE], heap2[HEAP_SIZE * 2];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, HEAP_SIZE);

    while (true) {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, HEAP_SIZE * 2);

    while (true) {
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

    return (hl_resize(heap, NULL, HEAP_SIZE / 2) != NULL);
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
    char data[2 * HEAP_SIZE + 32];
    void *heap = NULL;

    // do this test with 8 different heaps, each one
    // started with a different alignment
    bool hasnt_failed_yet = true;
    for (int i = 0; i < 8 && hasnt_failed_yet; i++) {
        // start heap at i bytes offset from 8 byte boundary
        heap = &data[i];
        hl_init(heap, 2 * HEAP_SIZE);
        DEBUG_PRINT_1("new heap offset = %d\n", i);

        // now alloc various sized blocks, and check each
        // for alignment
        for (int j = 1; j < 42 && hasnt_failed_yet; j++) {
            DEBUG_PRINT_1("-about to hl_alloc %d bytes...\n", j);
            char *ptr = hl_alloc(heap, j);
            DEBUG_PRINT_1("-hl_alloc returned: %p\n", ptr);
            if (ptr) {
                hasnt_failed_yet = (((uintptr_t) ptr) % 8 == 0);
            }
        }
    }

    return hasnt_failed_yet;
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

    char *array = hl_alloc(heap, (3 * HEAP_SIZE) / 4);
    hl_release(heap, array);

    return (hl_alloc(heap, (3 * HEAP_SIZE) / 4) != NULL);

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

    char *array = (char *) hl_alloc(heap, HEAP_SIZE / 4);

    for (int i = 0; i < HEAP_SIZE / 4; i++) {
        array[i] = 'A';
    }

    char *newarray = (char *) hl_resize(heap, array, HEAP_SIZE / 8);

    bool testfail = SUCCESS;

    for (int j = 0; j < HEAP_SIZE / 8; j++) {
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

    char *arrayA = (char*) hl_alloc(heap, HEAP_SIZE / 8);

    for (int i = 0; i < (HEAP_SIZE / 8) / sizeof (char); i++) {
        arrayA[i] = 'A';
    }

    char *arrayB = (char*) hl_alloc(heap, HEAP_SIZE / 8);

    for (int j = 0; j < (HEAP_SIZE / 8) / sizeof (char); j++) {
        arrayB[j] = 'B';
    }

    char *resizeArray = (char *) hl_resize(heap, arrayA, HEAP_SIZE / 4);

    bool testfail = SUCCESS;

    for (int k = 0; k < (HEAP_SIZE / 8) / sizeof (char); k++) {
        if (resizeArray[k] != 'A') {
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

    char data[HEAP_SIZE * 2];

    // start heap at 1 byte offset from 8-byte align point
    int offset = (9 - (((uintptr_t) & data[0]) % 8)) % 8;
    //offset = 0;
    void *heap = &data[offset];
    hl_init(heap, HEAP_SIZE * 2);

    int *array = hl_alloc(heap, HEAP_SIZE / 2);

    int *newarray = hl_resize(heap, array, (3 * HEAP_SIZE) / 4);

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
    char heap[MIN_HEAP_SIZE - 1];
    hl_init(heap, MIN_HEAP_SIZE - 1);

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
 * FUNCTIONS BEING TESTED: release
 * SPECIFICATION BEING TESTED: checks internal functionality
 * to see if release properly consolidates adjacent free
 * blocks.
 *
 * MANIFESTATION OF ERROR:
 * Final alloc can not find big enough block
 *
 */
#define TEST13_NBLOCKS 8

int test13() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    void *pointers[TEST13_NBLOCKS];
    memset(&pointers[0], 0, TEST13_NBLOCKS * sizeof (void *));

    int i;
    for (i = 0; i < TEST13_NBLOCKS; i++) {
        pointers[i] = hl_alloc(heap, HEAP_SIZE / TEST13_NBLOCKS);
        if (pointers[i] == NULL) break;
    }

    // there is no more room for a block. if we free 3 consecutive 
    // blocks, we should be able to allocate a 3x size block next.
    if (i < 5) return FAILURE;

    // we should have legit pointers in 0 - 5, all 128 bytes
    hl_release(heap, pointers[4]);
    hl_release(heap, pointers[2]);
    hl_release(heap, pointers[3]);
    // that last release should merge 3 free blocks

    void *big_block1 = hl_alloc(heap, 3 * HEAP_SIZE / TEST13_NBLOCKS);

    // now if we release 0, 1 and 5, we should just have that one
    // big block allocated. we should be able to alloc another 
    // double-size block where 0 and 1 were, and nother normal
    // block where 5 was. 
    hl_release(heap, pointers[0]);
    hl_release(heap, pointers[5]);
    hl_release(heap, pointers[1]); // this should cause merge

    void *big_block2 = hl_alloc(heap, 2 * HEAP_SIZE / TEST13_NBLOCKS);

    // now we should have just the one block left where 5 used to be, so 
    // this first alloc should work, second get null
    void *last_block = hl_alloc(heap, HEAP_SIZE / TEST13_NBLOCKS);

    bool it_worked = (big_block1 != NULL) && (big_block2 != NULL) &&
            (last_block != NULL);

    return (it_worked);
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_alloc
 * SPECIFICATION BEING TESTED: alloc size limit.
 * allocate block of max possible size = heap_size - sizeof heaphdr - sizeof blockhdr
 * (that calculation only works for 8 bit multiple heap size.)
 * MANIFESTATION OF ERROR:
 *
 */
int test14() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    
    unsigned int max_possible_size = HEAP_SIZE - 24 - 16-ALIGNMENT;
    max_possible_size = max_possible_size / ALIGNMENT * ALIGNMENT;
    
    void *ptr = hl_alloc(heap, max_possible_size);
    hl_release(heap, ptr);
    void *ptr2 = hl_alloc(heap, max_possible_size);
    hl_release(heap, ptr2);
    void *ptr3 = hl_alloc(heap, max_possible_size+1);

    return (ptr != NULL && ptr2 != NULL && ptr3 == NULL);
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
    int n_tries = 10000;
    int block_size = 22;

    // 1024 bytes of padding--fill it with Z's, check at end for change
    // --------------------
    // 1024 bytes of "heap"--fill all blocks allocated with x's
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"--fill it with A's, check at end for change
    char memarea[HEAP_SIZE * 3];

    char *heap_start = &memarea[HEAP_SIZE]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];

    // fill padding areas above and below heap with values we can
    // check at the end to see if any were overwritten
    memset(memarea, 'A', HEAP_SIZE);
    memset(memarea + HEAP_SIZE * 2, 'Z', HEAP_SIZE);

    // zero out the pointer array
    memset(pointers, 0, NPOINTERS * sizeof (char *));

    hl_init(heap_start, HEAP_SIZE);
    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0) {
            pointers[index] = hl_alloc(heap_start, block_size + (random() % 20));
            // if you got a block allocated, fill it with x's
            if (pointers[index] != NULL) {
                memset(pointers[index], 'x', block_size);
            }
        } else {
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
    for (int i = HEAP_SIZE * 2; i < HEAP_SIZE * 3 && overwrite_above == 0; i++) {
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

    char heap[HEAP_SIZE * 3];

    char *pointers[NPOINTERS];

    //zero out the pointer array
    memset(pointers, 0, NPOINTERS * sizeof (char *));

    hl_init(heap, HEAP_SIZE * 3);

    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        DEBUG_PRINT_3("i: %d, index: %d, ptr: %p\n", i, index, pointers[index]);
        if (pointers[index] == 0) {
            //NULL pointer, set pointer to heap
            pointers[index] = hl_alloc(heap, random() % (HEAP_SIZE * 2));
        } else {
            //resize the size of heap
            pointers[index] = hl_resize(heap, pointers[index], random() % (HEAP_SIZE * 2));
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

    char heap[HEAP_SIZE * 3];
    char *pointers[NPOINTERS];

    memset(pointers, 0, NPOINTERS * sizeof (char *));

    hl_init(heap, HEAP_SIZE * 3);

    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0) {
            pointers[index] = hl_alloc(heap, random() % (HEAP_SIZE * 2));
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

    char heap[HEAP_SIZE * 3];
    char *pointers[NPOINTERS];

    //zero out the pointer array
    memset(pointers, 0, NPOINTERS * sizeof (char *));

    hl_init(heap, HEAP_SIZE * 3);

    char *array = (char *) hl_alloc(heap, HEAP_SIZE);
    //set everything in array to A
    for (int i = 0; i < HEAP_SIZE; i++) {
        array[i] = 'A';
    }

    //set pointers[0] to array
    pointers[0] = array;

    srandom(0);
    for (int j = 0; j < n_tries; j++) {
        int index = random() % NPOINTERS;
        if (index != 0) {
            if (pointers[index] == 0) {
                pointers[index] = hl_alloc(heap, block_size);
                //change the contents in pointers[index] to B
                for (int t = 0; t < block_size && (pointers[index] != NULL); t++) {
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
    for (int k = 0; k < HEAP_SIZE; k++) {
        if (pointers[0][k] != 'A') {
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
    int block_size = 41;

    char heap[HEAP_SIZE * 3];
    char *pointers[NPOINTERS];

    //zero out the pointer array
    memset(pointers, 0, NPOINTERS * sizeof (char *));
    hl_init(heap, HEAP_SIZE * 3);
    char *array = (char *) hl_alloc(heap, HEAP_SIZE);

    //set everything in array to A
    for (int i = 0; i < HEAP_SIZE; i++) {
        array[i] = 'A';
    }

    //set pointers[0] to array
    pointers[0] = array;

    srandom(0);
    for (int j = 0; j < n_tries; j++) {
        int index = random() % NPOINTERS;
        if (index != 0) {
            if (pointers[index] == NULL) {
                pointers[index] = hl_alloc(heap, block_size);
                if (pointers[index] != NULL) {
                    memset(pointers[index], 'B', block_size);
                }
            } else {
                int number = random() % (block_size * 2);
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
        if (pointers[0][k] != 'A') {
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
 * FUNCTIONS BEING TESTED: All
 * INTEGRITY OR DATA CORRUPTION? Both
 *
 * MANIFESTATION OF ERROR:
 * If anything gets odd results: alloc, release, resize data integrity
 *
 */
int test23() {
#define TEST23_MAX_ALLOC_SIZE HEAP_SIZE
    int total_allocs = 0;
    int failed_allocs = 0;
    int total_releases = 0;
    int total_resizes = 0;
    int total_writes = 0;
    
    // data is 2 chunks of bottom check, 1 chunk heap, 2 chunks top check
    char alldata[5 * HEAP_SIZE + ALIGNMENT];
    char *bottom_check = &alldata[0];
    char *heap_data = &alldata[2 * HEAP_SIZE];
    char *top_check = &alldata[3 * HEAP_SIZE];

    void *heap = NULL;
    unsigned int block_size;

    srandom(0);

    // do this test with 8 different heaps, each one
    // started with a different alignment
    bool hasnt_failed_yet = true;
    for (int iheap = 0; iheap < 8 && hasnt_failed_yet; iheap++) {

        char *pointers[NPOINTERS];
        unsigned int sizes[NPOINTERS];

        int n_tries = 1500;

        // start heap at i bytes offset from 8 byte boundary
        heap = &heap_data[iheap];
        hl_init(heap, HEAP_SIZE);
        DEBUG_PRINT_1("new heap offset = %d\n", iheap);
        
        int heap_adjust_bytes = (ALIGNMENT - iheap) % ALIGNMENT;
        
        top_check = &alldata[3 * HEAP_SIZE + heap_adjust_bytes];
        memset(bottom_check, 'B', 2 * HEAP_SIZE);
        memset(top_check, 'T', 2 * HEAP_SIZE);

        
        memset(pointers, 0, NPOINTERS * sizeof (char *));
        memset(sizes, 0, NPOINTERS * sizeof (unsigned int));

        // now do a ton of operations on the heap
        for (int j = 0; j < n_tries && hasnt_failed_yet; j++) {
            int index = random() % NPOINTERS;
            DEBUG_PRINT_3("--Loop %d-%d, index %d\n", iheap, j, index);

            // if this ptr is null, try to alloc a new block with
            // either alloc or resize one in 5 times
            if (pointers[index] == NULL) {
                total_allocs++;
                block_size = random() % TEST23_MAX_ALLOC_SIZE + 1;
                if (random() % 5 != 2) {
                    DEBUG_PRINT_1("-about to alloc %d bytes...\n", block_size);
                    pointers[index] = hl_alloc(heap, block_size);
                    if (pointers[index] == NULL) {
                        failed_allocs++;
                    }
                } else {
                    DEBUG_PRINT_1("-about to resize new %d bytes...\n", block_size);
                    pointers[index] = hl_resize(heap, NULL, block_size);
                }
                // if we got a ptr, check alignment and write some data
                if (pointers[index] != NULL) {
                    hasnt_failed_yet = (((uintptr_t) pointers[index]) % ALIGNMENT == 0);
                    memset(pointers[index], ((int) 'a') + index % 26, block_size);
                    sizes[index] = block_size;
                } else {
                    sizes[index] = 0;
                }

            } else {
                // if it's an allocated block, you can write, resize or usually release
                int operation = random() % 16;
                if (operation == 0) {
                    total_writes++;
                    // write
                    DEBUG_PRINT_3("-about to write %d bytes to ptr[%d] at %p...\n",
                            sizes[index], index, pointers[index]);
                    memset(pointers[index], ((int) 'a') + index % 26, sizes[index]);
                } else if (operation == 1) {
                     // resize
                    total_resizes++;
                    unsigned int new_size = random() % TEST23_MAX_ALLOC_SIZE + 1;
                    DEBUG_PRINT_3("-about to resize ptr[%d] from %d to %d...\n",
                            index, sizes[index], new_size);
                    int bytes_to_check = MIN(new_size, sizes[index]);
                    pointers[index] = hl_resize(heap, pointers[index], new_size);
                    if (pointers[index] != NULL) {
                        sizes[index] = new_size;
                        //check alignment and that content is correct
                        hasnt_failed_yet = (((uintptr_t) pointers[index]) % ALIGNMENT == 0);
                        for (int n = 0; n < bytes_to_check && hasnt_failed_yet; n++) {
                            hasnt_failed_yet = (pointers[index][n] == (char) (((int) 'a') + index % 26));
                            if (!hasnt_failed_yet) {
                                DEBUG_PRINT_3("resize data fail: ptr[%d], byte %d, val=%c\n",
                                        index, n, pointers[index][n]);
                            }
                        }
                        // now fill out full block so future checks work right
                        memset(pointers[index], ((int) 'a') + index % 26, sizes[index]);
                    }
                    
                } else {
                    total_releases++;
                    // release
                    DEBUG_PRINT_3("-about to release %d bytes for ptr[%d] at %p...\n",
                            sizes[index], index, pointers[index]);
                    hl_release(heap, pointers[index]);
                    pointers[index] = NULL;
                    sizes[index] = 0;
                    
                } // end release
            } // end allocated block

            //DEBUG_PRINT("Checking bottom zone.\n");
            for (int k = 0; k < HEAP_SIZE * 2 && hasnt_failed_yet; k++) {
                hasnt_failed_yet = (alldata[k] == 'B');
                if (!hasnt_failed_yet) {
                    DEBUG_PRINT_2("Bottom check fail at byte %d, val = %c\n", k, alldata[k]);
                }
            }

            //DEBUG_PRINT("Checking top zone.\n");
            for (int k = 0; k < HEAP_SIZE * 2 && hasnt_failed_yet; k++) {
                hasnt_failed_yet = (top_check[k] == 'T');
                if (!hasnt_failed_yet) {
                    DEBUG_PRINT_2("Top check fail at byte %d, val = %c\n",
                            k, top_check[k]);
                }
            }

            // check every allocated pointer
            for (int p = 0; p < NPOINTERS && hasnt_failed_yet; p++) {
                if (pointers[p] != NULL) {
                    for (int q = 0; q < sizes[p] && hasnt_failed_yet; q++) {
                        hasnt_failed_yet = (pointers[p][q] == (char) (((int) 'a') + p % 26));
                        if (!hasnt_failed_yet) {
                            DEBUG_PRINT_3("data check fail: ptr[%d], byte %d, val=%c ",
                                    p, q, pointers[p][q]);
                            DEBUG_PRINT_2("not %c, size %d\n",
                                    (char) (((int) 'a') + p % 26), sizes[p]);

                        }
                    }
                }
            }

        } // end ntries loop
        DEBUG_PRINT_3("allocs: %d, resizes: %d, releases: %d\n", total_allocs, total_resizes, total_releases);
        DEBUG_PRINT_2("writes: %d, failed allocs: %d\n", total_writes, failed_allocs);
    }

    return hasnt_failed_yet;
}
