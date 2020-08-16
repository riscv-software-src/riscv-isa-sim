#include "riscv/mmio_plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
 #include <sys/mman.h>
int len;
void* test_mmio_plugin_alloc(const char* args)
{
    printf("ALLOC -- ARGS=%s\n", args);
    int fd = open(args, O_RDWR, 0);
    if(fd==-1){
      printf("open block disk file failed.\n");
      exit(-1);
    }
    struct stat st; //定义文件信息结构体
    int r=fstat(fd,&st);
    if(r==-1){
      printf("get file size failed. \n");
      close(fd);
      exit(-1);
    }
    len=st.st_size;
    void * p=mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);  
    return p;
}

bool test_mmio_plugin_load(void* self, reg_t addr, size_t len, uint8_t* bytes)
{
    //printf("LOAD -- SELF=%p ADDR=0x%lx LEN=%lu BYTES=%d\n", self, addr, len, bytes[0]);
    memcpy(bytes, (char *)self + addr, len);
    return true;
}

bool test_mmio_plugin_store(void* self, reg_t addr, size_t len, const uint8_t* bytes)
{
    //printf("STORE -- SELF=%p ADDR=0x%lx LEN=%lu BYTES=%p\n", self, addr, len, (const void*)bytes);
    memcpy((char *)self + addr, bytes, len);
    msync((char *)self + addr,len,0);
    return true;
}

void test_mmio_plugin_dealloc(void* self)
{
    munmap(self, len);
    printf("DEALLOC -- SELF=%p\n", self);
}

__attribute__((constructor)) static void on_load()
{
  static mmio_plugin_t test_mmio_plugin = {
      test_mmio_plugin_alloc,
      test_mmio_plugin_load,
      test_mmio_plugin_store,
      test_mmio_plugin_dealloc
  };

  register_mmio_plugin("test_mmio_plugin", &test_mmio_plugin);
}
