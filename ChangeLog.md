Version 1.1.0
-------------
- Zbkb, Zbkc, Zbkx, Zknd, Zkne, Zknh, Zksed, Zksh scalar cryptography extensions (Zk, Zkn, and Zks groups), v1.0
- Zkr virtual entropy source emulation, v1.0
- V extension, v1.0
- P extension, v0.9.2
- Zba extension, v1.0
- Zbb extension, v1.0
- Zbc extension, v1.0
- Zbs extension, v1.0
- Hypervisor extension, v1.0
- Svnapot extension, v1.0
- Svpbmt extension, v1.0
- Svinval extension, v1.0

Version 1.0.1-dev
-----------------
- Preliminary support for a subset of the Vector Extension, v0.7.1.
- Support S-mode vectored interrupts (i.e. `stvec[0]` is now writable).
- Added support for dynamic linking of libraries containing MMIO devices.
- Added `--priv` flag to control which privilege modes are available.
- When the commit log is enabled at configure time (`--enable-commitlog`),
  it must also be enabled at runtime with the `--log-commits` option.
- Several debug-related additions and changes:
  - Added `hasel` debug feature.
  - Added `--dm-no-abstract-csr` command-line option.
  - Added `--dm-no-halt-groups` command line option.
  - Renamed `--progsize` to `--dm-progsize`.
  - Renamed `--debug-sba` to `--dm-sba`.
  - Renamed `--debug-auth` to `--dm-auth`.
  - Renamed `--abstract-rti` to `--dm-abstract-rti`.
  - Renamed `--without-hasel` to `--dm-no-hasel`.

Version 1.0.0 (2019-03-30)
--------------------------
- First versioned release.
