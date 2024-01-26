#include <stdio.h>

int main(int argc, char **argv)
{
  printf("Hello World!\n");
  printf("a icin deger girin\n");

  int a;
  scanf("%d", &a);


  FILE *fp;
  fp = fopen("/home/usr1/riscv-isa-sim/a_tets_faruk/spike_link/log/dummy.txt", "w");
  printf("%p\n", fp);
  fprintf(fp, "%d", a);
  fclose(fp);
}
