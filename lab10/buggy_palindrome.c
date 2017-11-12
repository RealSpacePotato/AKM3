R#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


bool is_palindrome_helper(char *s, int len, int c1, int c2) {
    if(c1 < len && c2 >= 0) {
        // We don't count non-alphanumerics
        // Ignore case - just make everything lowercase
        char c1_char = s[c1];
        while(!(isalnum(c1_char))) {
            c1++;
	if (c1<len){
            c1_char = s[c1];}
        }
        c1_char = tolower(c1_char);
        char c2_char = s[c2];
        while(!(isalnum(c2_char))) {
            c2--;
	if (c2>=0){
            c2_char = s[c2];
        }}
        c2_char = tolower(c2_char);
        // If they're not equal, it's not a palindrome
        if((c1_char != c2_char)&&(isalnum(c1_char))&&(isalnum(c2_char))) {
            return false;
        }
        // If they are equal, it might still be a palindrome
        else {
            c1++;
            c2--;
            return is_palindrome_helper(s, len, c1, c2);
        }
    }
    // If we reached the end, they must be palindromes
    else {
        return true;
    }
}

bool is_palindrome(char *s) {
    int len = strlen(s);
    int c1 = 0;
    int c2 = len - 1;
    return is_palindrome_helper(s, len, c1, c2);
}

// DO NOT CHANGE THE MAIN FUNCTION - THERE ARE NO INTENTIONAL BUGS TO FIX HERE
// Any changes to the main function will potentially break the autograder
int main(int argc, char *argv[]) {
    if(argc != 2) { // First element of the array is always the program
        printf("I take in 1 argument and return whether or not it is a palindome \n");
        return 1;
    }

    char *s= argv[1];
    printf("String: %s\n", s);

    if(is_palindrome(s)) {
        printf("Yes, it is a palindrome\n");
    }
    else {
        printf("No, it is not a palindrome\n");
    }
    return 0;
}
