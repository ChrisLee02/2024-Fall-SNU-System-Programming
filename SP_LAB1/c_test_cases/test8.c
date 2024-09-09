#include <stdio.h>
#include <stdlib.h>

/* Author: Seongjong Bae */
/* Testing strings and comments */

int main() {
    printf("This string contains a comment symbol: /* not a comment */\n");
    printf("Another string with a // single-line comment inside.\n");

    /* Comment in the middle */ printf("This should still compile correctly.\n");

    // Single-line comment with a multi-line pattern: /* still just a single-line comment */
    
    return EXIT_SUCCESS;
}
