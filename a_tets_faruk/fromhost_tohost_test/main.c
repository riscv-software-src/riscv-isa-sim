#include <stdio.h>
// volatile long unsigned int tohost;
// volatile long unsigned int fromhost;
int main(int argc, char **argv)
{
  // for (int i = 0; i < 10; i++)
  //   fromhost = 2; // bu sekilde yapmak hataya sebep olmadi.
  
  printf("a icin deger girin\n");

  int a;
  scanf("%d", &a);


  FILE *fp;
  fp = fopen("dummy.txt", "w");
  printf("%p\n", fp);
  fprintf(fp, "%d", a);
  fclose(fp);
}
