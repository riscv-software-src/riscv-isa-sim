#include <stdio.h>
volatile long unsigned int tohost;
volatile long unsigned int fromhost;
int main(int argc, char **argv)
{
  // for (int i = 0; i < 10; i++)
  //   fromhost = 2; // bu sekilde yapmak hataya sebep olmadi.
  
  // !!! burda tohost kismi calistiriliyor olmali
  printf("Hello World!\n");

  // !!! burda fromhost kismi
  int a;
  scanf("%d", &a);
}
