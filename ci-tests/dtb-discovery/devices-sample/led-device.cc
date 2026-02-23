#include "led-device.h"
#include <cstring>
#include <cstdio>
#include <sstream>
#include <algorithm>

led_device_t::led_device_t(const sim_t *sim, reg_t base, size_t size, uint32_t num_leds)
    : sim(sim), base_addr(base), dev_size(size), 
      num_leds(std::min(num_leds, 32u)), led_state(0) {
  printf("LED device init at 0x%lx, size: 0x%lx, num_leds: %u\n", 
         base_addr, dev_size, this->num_leds);
}

bool led_device_t::load(reg_t addr, size_t len, uint8_t* bytes) {
  printf("led_device_t::load from address: 0x%lx, length: %zu\n",
         base_addr + addr, len);
  
  if (addr == 0 && len == 4) {
    uint32_t masked_state = led_state & ((1u << num_leds) - 1);
    memcpy(bytes, &masked_state, 4);
    return true;
  }
  return false;
}

bool led_device_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
  printf("led_device_t::store to address: 0x%lx, length: %zu\n",
         base_addr + addr, len);
  
  if (addr == 0 && len == 4) {
    uint32_t new_state;
    memcpy(&new_state, bytes, 4);
    led_state = new_state & ((1u << num_leds) - 1);
    
    printf("LED state changed to: 0x%x\n", led_state);
    printf("LED pattern (%u LEDs): ", num_leds);
    for (int i = num_leds - 1; i >= 0; i--) {
      printf("%c", (led_state & (1 << i)) ? ' ' : '-');
    }
    printf(" (0x%x)\n", led_state);
    return true;
  }
  return false;
}

size_t led_device_t::size() {
  return dev_size;
}

led_device_t* led_parse_from_fdt(const void *fdt, const sim_t *sim,
                                 reg_t *base, std::vector<std::string> args) {
  if (args.size() < 3) {
    fprintf(stderr, "Error: led device requires 3 arguments (base, size, num_leds)\n");
    return nullptr;
  }
  
  try {
    *base = std::stoull(args[0], nullptr, 0);
    reg_t size = std::stoull(args[1], nullptr, 0);
    uint32_t num_leds = std::stoul(args[2], nullptr, 0);
    
    if (num_leds == 0 || num_leds > 32) {
      fprintf(stderr, "Error: num_leds must be between 1 and 32, got %u\n", num_leds);
      return nullptr;
    }
    
    return new led_device_t(sim, *base, size, num_leds);
  } catch (const std::exception& e) {
    fprintf(stderr, "Error parsing LED device params: %s\n", e.what());
    return nullptr;
  }
}

std::string led_generate_dts(const sim_t *sim, const std::vector<std::string> &args) {
  std::ostringstream s;
  reg_t base = 0x10001000;
  reg_t size = 0x1000;
  uint32_t num_leds = 8;
  
  if (args.size() >= 3) {
    base = std::stoull(args[0], nullptr, 0);
    size = std::stoull(args[1], nullptr, 0);
    num_leds = std::stoul(args[2], nullptr, 0);
  }
  
  s << std::hex
    << "    led@" << base << " {\n"
    << "      compatible = \"led\";\n"
    << "      reg = <0x0 0x" << base << " 0x0 0x" << size << ">;\n"
    << std::dec
    << "      num-leds = <" << num_leds << ">;\n"
    << std::hex
    << "      spike,plugin-params = \"0x" << base << ",0x" << size << "," << std::dec << num_leds << "\";\n"
    << "    };\n";
  
  return s.str();
}

REGISTER_DEVICE(led_device, led_parse_from_fdt, led_generate_dts)
