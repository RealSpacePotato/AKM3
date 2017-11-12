#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void **get_bytestring1(int *byte_len);
void **get_bytestring2(int *byte_len);
void **get_bytestring3(int *byte_len);

struct bs2_struct {
    char c1;
    int i;
    /* TODO: determine what type of pointer these are */
    void *ptr;
} typedef bs2_struct;

void print_char_array(char *arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("%c", arr[i]);
    }
    printf("\n");
}

void print_int_array(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("%i ", arr[i]);
    }
    printf("\n");
}

void print_float_array(float *arr, int len) {
    for (int i = 0; i < len; i++) {
        printf("%f ", arr[i]);
    }
    printf("\n");
}

void print_bs2_struct_array(bs2_struct *arr, int len) {
    int i;
    printf("Bytestring 2: printing each arr[i].c1 as a char:\n");
    for (i = 0; i < len; i++) {
        printf("%c", arr[i].c1);
    }
    printf("\n");

    printf("Bytestring 2: printing each arr[i].i as an int:\n");
    for (i = 0; i < len; i++) {
        printf("%d ", arr[i].i);
    }
    printf("\n");

    /* TODO: Print what these point to instead of printing them as addresses. */
    printf("Bytestring 2: printing string of i chars starting at at arr[i].ptr:\n");
    for (i = 0; i < len; i++) {
	char *str = (char *) (arr[i].ptr);
        printf("%c", str[i]);
    }
    printf("\n");
}

void interp1(void **bytestring, size_t bytes) {
    printf("Bytestring 1 as an int array:\n");
    int *int_arr = (int *)bytestring;
    int int_len = bytes / sizeof(int) + bytes % sizeof(int);
    print_int_array(int_arr, int_len);
    printf("\n");

    printf("Bytestring 1 as a float array:\n");
    float *float_arr = (float *)bytestring;
    int float_len = bytes / sizeof(float) + bytes % sizeof(float);
    print_float_array(float_arr, float_len);
    printf("\n");

    printf("Bytestring 1 as a char array:\n");
    char *char_arr = (char *)bytestring;
    int char_len = bytes / sizeof(char) + bytes % sizeof(char);
    print_char_array(char_arr, char_len);
    printf("\n");
}

void interp2(void **bytestring, size_t bytes) {
    int len = bytes / sizeof(bs2_struct) + bytes % sizeof(bs2_struct);
    print_bs2_struct_array((bs2_struct *)bytestring, len);
}

struct mystery_struct {
    /* TODO: rewrite this definition. */
    int stupidInt;
    char usefulChar;
    int moreStupidInts[6];
} typedef mystery_struct;

void print_mystery(mystery_struct **arr, int len) {
    int i;
    /* TODO: display arr as an array of pointers to mystery_structs. */
    //printf("Printing bytestring3 as array of %d mystery_structs:\n", len);
    for (i=0; i < len && arr[i] != NULL; i++) {
	mystery_struct *ms = arr[i];
	printf("%c", ms->usefulChar);
	//for (int j = 0; j < 32; j++) {
	//    printf("%c ", ms->c[j]);
	//}
    }
    printf("\nDone\n");
}

void interp3(void **bytestring, size_t bytes) {
    int len = bytes / sizeof(mystery_struct *);
    print_mystery((mystery_struct **)bytestring, len);
/*
    printf("Bytestring 3 starts at: %p\n", bytestring);
    printf("Bytestring 3 as a pointer array:\n");
    int len = bytes / sizeof(void *);
    for (int i = 0; i < len; i++) {
	printf("%p ", bytestring[i]);
    }
    printf("\n");
    */
}

int main(int argv, char **argc) {
    int bytes_1;
    void **bytestring_1 = get_bytestring1(&bytes_1);
    //printf("Address of bytestring 1: %p\n", bytestring_1);
    //interp1(bytestring_1, bytes_1);

    int bytes_2;
    void **bytestring_2 = get_bytestring2(&bytes_2);
    //interp2(bytestring_2, bytes_2);

    int bytes_3;
    void ** bytestring_3 = get_bytestring3(&bytes_3);
    interp3(bytestring_3, bytes_3);

    return 0;
}
