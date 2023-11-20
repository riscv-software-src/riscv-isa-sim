#include <stdio.h>

int array[10];

int main() {
    int a = 0;

    FILE* read_write = fopen("floating_file","r+");
    fprintf(read_write,"5\n");
    fflush(read_write);
    rewind(read_write);
    scanf("%d\n",&a);
    printf("a after first scan: %d\n",a);
    fflush(stdin);
    fflush(stdout);
    fscanf(read_write,"%d\n",&a);
    printf("a after second scan: %d\n",a);

    return a;
}
