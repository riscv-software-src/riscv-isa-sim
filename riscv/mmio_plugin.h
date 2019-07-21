#ifndef _RISCV_MMIO_PLUGIN_H
#define _RISCV_MMIO_PLUGIN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint64_t reg_t;

typedef struct {
  // Allocate user data for an instance of the plugin. The parameter is a simple
  // c-string containing arguments used to construct the plugin. It returns a
  // void* to the allocated data.
  void* (*alloc)(const char*);

  // Load a memory address of the MMIO plugin. The parameters are the user_data
  // (void*), memory offset (reg_t), number of bytes to load (size_t), and the
  // buffer into which the loaded data should be written (uint8_t*). Return true
  // if the load is successful and false otherwise.
  bool (*load)(void*, reg_t, size_t, uint8_t*);

  // Store some bytes to a memory address of the MMIO plugin. The parameters are
  // the user_data (void*), memory offset (reg_t), number of bytes to store
  // (size_t), and the buffer containing the data to be stored (const uint8_t*).
  // Return true if the store is successful and false otherwise.
  bool (*store)(void*, reg_t, size_t, const uint8_t*);

  // Deallocate the data allocated during the call to alloc. The parameter is a
  // pointer to the user data allocated during the call to alloc.
  void (*dealloc)(void*);
} mmio_plugin_t;

// Register an mmio plugin with the application. This should be called by
// plugins as part of their loading process.
extern void register_mmio_plugin(const char* name_cstr,
                                 const mmio_plugin_t* mmio_plugin);

#ifdef __cplusplus
}

#include <string>

// Wrapper around the C plugin API that makes registering a C++ class with
// correctly formed constructor, load, and store functions easier. The template
// type should be the type that implements the MMIO plugin interface. Simply
// make a global mmio_plugin_registration_t and your plugin should register
// itself with the application when it is loaded because the
// mmio_plugin_registration_t constructor will be called.
template <typename T>
struct mmio_plugin_registration_t
{
  static void* alloc(const char* args)
  {
    return reinterpret_cast<void*>(new T(std::string(args)));
  }

  static bool load(void* self, reg_t addr, size_t len, uint8_t* bytes)
  {
    return reinterpret_cast<T*>(self)->load(addr, len, bytes);
  }

  static bool store(void* self, reg_t addr, size_t len, const uint8_t* bytes)
  {
    return reinterpret_cast<T*>(self)->store(addr, len, bytes);
  }

  static void dealloc(void* self)
  {
    delete reinterpret_cast<T*>(self);
  }

  mmio_plugin_registration_t(const std::string& name)
  {
    mmio_plugin_t plugin = {
      mmio_plugin_registration_t<T>::alloc,
      mmio_plugin_registration_t<T>::load,
      mmio_plugin_registration_t<T>::store,
      mmio_plugin_registration_t<T>::dealloc,
    };

    register_mmio_plugin(name.c_str(), &plugin);
  }
};
#endif // __cplusplus

#endif
