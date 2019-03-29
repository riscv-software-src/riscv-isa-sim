// See LICENSE file for license details.

#ifndef DEBUG_ROM_DEFINES_H
#define DEBUG_ROM_DEFINES_H

// These are implementation-specific addresses in the Debug Module
#define DEBUG_ROM_HALTED    0x100
#define DEBUG_ROM_GOING     0x104
#define DEBUG_ROM_RESUMING  0x108
#define DEBUG_ROM_EXCEPTION 0x10C

// Region of memory where each hart has 1
// byte to read.
#define DEBUG_ROM_FLAGS 0x400
#define DEBUG_ROM_FLAG_GO     0
#define DEBUG_ROM_FLAG_RESUME 1

// These needs to match the link.ld         
#define DEBUG_ROM_WHERETO 0x300
#define DEBUG_ROM_ENTRY   0x800
#define DEBUG_ROM_TVEC    0x808

#endif
