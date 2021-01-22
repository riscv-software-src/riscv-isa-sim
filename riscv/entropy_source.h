
#include <fstream>
#include <iostream>

#include "internals.h"

//
// Used to model the cryptography extension entropy source.
// See Section 4 of the Scalar Cryptography Extension specificaiton.
class entropy_source {

public:

  // Valid return codes for OPST bits [31:30] when reading mentropy.
  static const uint32_t OPST_BIST = 0x0 << 30;
  static const uint32_t OPST_ES16 = 0x1 << 30;
  static const uint32_t OPST_WAIT = 0x2 << 30;
  static const uint32_t OPST_DEAD = 0x3 << 30;

  //
  // Other system events
  // ------------------------------------------------------------

  void reset() {
    // Does nothing for now. In the future, can be used to model things
    // like initial BIST states.
  }

  //
  // mentropy register
  // ------------------------------------------------------------

  void set_mentropy(reg_t val) {
    // Always ignore writes to mentropy.
    // This CSR *must never* accept write values, it is strictly read only.
  }


  //
  // The format of mentropy is described in Table 3 / Section 4.1 of
  // the scalar cryptography specification.
  reg_t get_mentropy() {

    uint32_t result  = 0;

    // Currently, always return ES16 (i.e. good randomness) unless in
    // noise capture mode.  In the future, we can more realistically model
    // things like WAIT states, BIST warm up and maybe scriptable entry
    // into the DEAD state, but until then, this is the bare minimum.
    uint32_t return_status = noise_capture_mode ? OPST_BIST : OPST_ES16;

    if(return_status == OPST_ES16) {

        // Add some sampled entropy into the low 16 bits
        uint16_t seed  = this -> get_two_random_bytes();
        result        |= seed;

    } else if(return_status == OPST_BIST) {

        // Do nothing.

    } else if(return_status == OPST_WAIT) {

        // Do nothing.

    } else if(return_status == OPST_DEAD) {

        // Do nothing. Stay dead.

    } else {

        // Unreachable.

    }

    // Note that if we get here any return_status is anything other than
    // OPST_ES16, then the low 16-bits of the return value must be zero.

    result |= return_status;

    // Result is zero-extended on RV64.
    return (reg_t)result;
  }

  //
  // mnoise register
  // ------------------------------------------------------------


  void set_mnoise(reg_t val) {
      // Almost all of the behaviour for mnoise is vendor specific,
      // except for bit 31.
      int new_noisemode = (val >> 31) & 0x1;
      noise_capture_mode = new_noisemode == 1;
  }


  reg_t get_mnoise() {
      reg_t to_return = 0;

      if(this -> noise_capture_mode) {
          // Set bit 31 indicating we are in noise capture mode.
          to_return |= 0x1 << 31;
      }

      return to_return;
  }

  //
  // Utility / support variables and functions.
  // ------------------------------------------------------------

  // The ES is in noise capture mode?
  // If so, then get_mentropy must always return OPST_BIST.
  bool noise_capture_mode = false;

  // The file to read entropy from.
  std::string randomness_source = "/dev/urandom";

  // Read two random bytes from the entropy source file.
  uint16_t get_two_random_bytes() {

      std::ifstream fh(this -> randomness_source, std::ios::binary);

      if(fh.is_open()) {

          uint16_t random_bytes;

          fh.read((char*)(&random_bytes), 2);

          fh.close();

          return random_bytes;

      } else {

          fprintf(stderr, "Could not open randomness source file:\n\t");
          fprintf(stderr, "%s", randomness_source.c_str());
          abort();

      }

  }

};

