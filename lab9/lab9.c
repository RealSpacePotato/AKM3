#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct arraylist {
    void** buffer;            // pointer to allocated memory
    unsigned int buffer_size; // the max number of elements the buffer can hold
    unsigned int length;      // the number of integers stored in the list
} arraylist;

////////////////////////////////////////////////////////////////////////////////
//Functions that you need to implement:

/*
 * Append the value x to the end of the arraylist. If necessary, double the
 * capacity of the arraylist.
 */
void arraylist_add(arraylist* a, void* x)
{

if (length+1 > *a.buffer_size){
	a = realloc(a, 2*(*a.buffer_size));
}
a_ptr[*a.length] = x;
*a.length = *a.length + 1;
	
}


/*
 * Store x at the specified index of the arraylist. Previously stored values
 * should be moved back rather than overwritten. It is undefined behavior to
 * insert an element with an index beyond the end of an arraylist.
 */
void arraylist_insert(arraylist* a, unsigned int index, void* x)
{
*a.buffer_size= *a.buffer_size+1;
int *b_ptr = (int*)malloc(*a.buffer_size);
memmove(b_ptr, a_ptr, index);
arraylist_add(b_ptr,x);
for (int i= 0; index+i< *a.length; i++){
arraylist_add(b_ptr,a[index+i]);}
a = realloc(b_ptr, *a.buffersize);

	

// Hint: Consider how you could implement this function in terms of
    // arraylist_add()

}

/*
 * Free any memory used by that arraylist.
 */
void arraylist_free(arraylist* a)
{   
 free(*a.buffer);
 free(a);
// Hint: How many times is malloc called when creating a new arraylist?
}

////////////////////////////////////////////////////////////////////////////////

arraylist* arraylist_new()
{
    arraylist* a = (arraylist*)malloc(sizeof(arraylist));
    a->buffer = (void**)malloc(4 * sizeof(void*));
    a->buffer_size = 4;
    a->length = 0;

    return a;
}

void arraylist_remove(arraylist* a, unsigned int index)
{
    for (unsigned int i = index; i < a->length - 2; i++)
        a->buffer[i] = a->buffer[i + 1];

    --a->length;
}

void* arraylist_get(arraylist* a, unsigned int index)
{
    return a->buffer[index];
}

void arraylist_print(arraylist* a)
{
    printf("[");
    if (a->length > 0) {
        for (unsigned int i = 0; i < a->length - 1; i++)
            printf("%d, ", *(int*)arraylist_get(a, i));
        printf("%d", *(int*)arraylist_get(a, a->length - 1));
    }

    printf("]\n");
}

int main(int argc, char* argv[])
{
    // START OF TEST
    arraylist* a = arraylist_new();
    int ints[] = { 0, 1, 2, 3, 4, 5, 100 };

    arraylist_add(a, ints);
    arraylist_add(a, ints + 1);
    arraylist_add(a, ints + 2);
    arraylist_add(a, ints + 3);
    arraylist_add(a, ints + 4);
    arraylist_add(a, ints + 5);
    arraylist_print(a);

    for (unsigned int i = 0; i < a->length; i++) {
        arraylist_insert(a, i, ints + 6);
        printf("Insert position %d: ", i);
        arraylist_print(a);
        arraylist_remove(a, i);
    }
    printf("Clean: ");
    arraylist_print(a);

    arraylist_free(a);
    // END OF TEST

    return 0;
}
