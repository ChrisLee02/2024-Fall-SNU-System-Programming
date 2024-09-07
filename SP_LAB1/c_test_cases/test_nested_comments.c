#include <stdio.h>

int main() {
    /* This comment starts here
    /* This is another comment inside the first comment */
    This should not be part of the code;
    */
    printf("If nested comments were supported, this wouldn't compile.
");
    return 0;
}
