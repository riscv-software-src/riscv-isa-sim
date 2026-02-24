#ifndef LED_DEVICE_H
#define LED_DEVICE_H

#include <riscv/sim.h>
#include <vector>
#include <string>

class led_device_t : public abstract_device_t {
public:
  led_device_t(const sim_t *sim, reg_t base, size_t size, uint32_t num_leds);
  
  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  size_t size() override;
  
private:
  const sim_t *sim;
  reg_t base_addr;
  size_t dev_size;
  uint32_t num_leds;
  uint32_t led_state;
};

 

#endif // LED_DEVICE_H
