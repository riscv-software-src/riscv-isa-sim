#include <stdio.h>
#include <string.h>

#define SIZE 100000

int arr[SIZE];
int main()
{

    for (int i = 0; i < SIZE; i++)
    {
        arr[i] = 0;
    }
    int toplam = 0;
    for (int i = 0; i < SIZE; i++)
    {
        toplam += arr[i];
    }
    printf("%d", arr[8]);
}
