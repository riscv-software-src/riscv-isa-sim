#define COSIMIF
#define COSIMIF_POLIMORPH_TEST
#include "htif.h"

void htif_t::use_idle(){
  // std::cout << "htif_t::use_idle calling idle" << std::endl;
  // idle();

  std::cout << "htif_t::use_idle calling idle_single_step" << std::endl;
  idle_single_step();
}

class expose_t : public htif_t{
  public: 
    void use_idle();

    ~expose_t();
    expose_t() : htif_t() {}
  
  private:
    virtual void idle_single_step() override; // !!! ekleme
    virtual void idle() override; // override etmezsek htif_t::idle W olarak yer aliyor.
    
    
    virtual endianness_t get_target_endianness() const override;
    virtual size_t chunk_align() override { return 8; }
    virtual size_t chunk_max_size() override { return 8; }
    virtual void read_chunk(addr_t taddr, size_t len, void* dst) override;
    virtual void write_chunk(addr_t taddr, size_t len, const void* src) override;
    virtual void reset() override;
};

void expose_t::use_idle(){ //boyle tanimlayince T oluyor.
  std::cout<<"expose_t::use_idle:";
  std::cout << " object at "<<this<<" of type expose_t.\n"
  " calling htif_t::use_idle" << std::endl;
  htif_t::use_idle();
}

void expose_t::idle(){
  std::cout << "expose_t::idle" << std::endl;
}

void expose_t::idle_single_step(){
  std::cout << "expose_t::idle_single_step" << std::endl;
}

std::function <void()> use_idle_callback;
expose_t* ex;
void init(){
  ex = new expose_t();
  std::cout << "init:\n"
  " expose_t object created at   "<<ex<<std::endl;
  use_idle_callback = std::bind(&expose_t::use_idle, ex);
  
}

void step(){
  use_idle_callback();

}
int main(int argc, char**argv){
  init();
  // ex->use_idle();
  step();
}




expose_t::~expose_t(){}

endianness_t expose_t::get_target_endianness() const{
  std::cout << "expose_t::get_target_endianness" << std::endl;
  return endianness_t::endianness_little;
}

void expose_t::read_chunk(addr_t taddr, size_t len, void* dst){
  std::cout << "expose_t::read_chunk" << std::endl;
}

void expose_t::write_chunk(addr_t taddr, size_t len, const void* src){
  std::cout << "expose_t::write_chunk" << std::endl;
}

void expose_t::reset(){
  std::cout << "expose_t::reset" << std::endl;
}