#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern size_t CONFIG_MSIZE;
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

size_t load_from_file(char *filename, uint8_t *data) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    printf("Can not open '%s'\n", filename);
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  size_t img_size = ftell(fp);
  if (img_size > CONFIG_MSIZE) {
    img_size = CONFIG_MSIZE;
  }

  fseek(fp, 0, SEEK_SET);
  fread(data, img_size, 1, fp);

  fclose(fp);

  return img_size;
}

int main(int argc, char *argv[]) {
  uint8_t *data = malloc(CONFIG_MSIZE);
  for (int i = 1; i < argc; i++) {
    printf("Running %s\n", argv[i]);
    size_t size = load_from_file(argv[i], data);
    LLVMFuzzerTestOneInput(data, size);
  }
  free(data);
  return 0;
}
