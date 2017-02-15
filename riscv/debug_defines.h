#define AC_ACCESS_REGISTER                  None
/*
* 2: Access the lowest 32 bits of the register.
*
* 3: Access the lowest 64 bits of the register.
*
* 4: Access the lowest 128 bits of the register.
*
* If \Fsize specifies a size larger than the register is, then the
* access must fail. If a register is accessible, then \Fsize matching
* the register's actual size must be supported.
 */
#define AC_ACCESS_REGISTER_SIZE_OFFSET      19
#define AC_ACCESS_REGISTER_SIZE_LENGTH      3
#define AC_ACCESS_REGISTER_SIZE             (0x7 << AC_ACCESS_REGISTER_SIZE_OFFSET)
/*
* When 1, execute the program in the Program Buffer exactly once
* before performing the read/write.
 */
#define AC_ACCESS_REGISTER_PREEXEC_OFFSET   18
#define AC_ACCESS_REGISTER_PREEXEC_LENGTH   1
#define AC_ACCESS_REGISTER_PREEXEC          (0x1 << AC_ACCESS_REGISTER_PREEXEC_OFFSET)
/*
* When 1, execute the program in the Program Buffer exactly once
* after performing the read/write.
 */
#define AC_ACCESS_REGISTER_POSTEXEC_OFFSET  17
#define AC_ACCESS_REGISTER_POSTEXEC_LENGTH  1
#define AC_ACCESS_REGISTER_POSTEXEC         (0x1 << AC_ACCESS_REGISTER_POSTEXEC_OFFSET)
/*
* 0: Copy data from {\tt arg0} portion of {\tt data} into the
* specified register.
*
* 1: Copy data from the specified register into {\tt arg0} portion
* of {\tt data}.
 */
#define AC_ACCESS_REGISTER_WRITE_OFFSET     16
#define AC_ACCESS_REGISTER_WRITE_LENGTH     1
#define AC_ACCESS_REGISTER_WRITE            (0x1 << AC_ACCESS_REGISTER_WRITE_OFFSET)
/*
* Number of the register to access, as described in Table~\ref{tab:regno}.
 */
#define AC_ACCESS_REGISTER_REGNO_OFFSET     0
#define AC_ACCESS_REGISTER_REGNO_LENGTH     16
#define AC_ACCESS_REGISTER_REGNO            (0xffff << AC_ACCESS_REGISTER_REGNO_OFFSET)
#define AC_QUICK_ACCESS                     None
#define AC_QUICK_ACCESS_1_OFFSET            24
#define AC_QUICK_ACCESS_1_LENGTH            8
#define AC_QUICK_ACCESS_1                   (0xff << AC_QUICK_ACCESS_1_OFFSET)
#define CSR_DCSR                            0x7b0
/*
* 0: There is no external debug support.
*
* 1: External debug support exists as it is described in this document.
*
* Other values are reserved for future standards.
 */
#define CSR_DCSR_XDEBUGVER_OFFSET           30
#define CSR_DCSR_XDEBUGVER_LENGTH           2
#define CSR_DCSR_XDEBUGVER                  (0x3 << CSR_DCSR_XDEBUGVER_OFFSET)
/*
* When 1, {\tt ebreak} instructions in Machine Mode enter Halt Mode.
 */
#define CSR_DCSR_EBREAKM_OFFSET             15
#define CSR_DCSR_EBREAKM_LENGTH             1
#define CSR_DCSR_EBREAKM                    (0x1 << CSR_DCSR_EBREAKM_OFFSET)
/*
* When 1, {\tt ebreak} instructions in Hypervisor Mode enter Halt Mode.
 */
#define CSR_DCSR_EBREAKH_OFFSET             14
#define CSR_DCSR_EBREAKH_LENGTH             1
#define CSR_DCSR_EBREAKH                    (0x1 << CSR_DCSR_EBREAKH_OFFSET)
/*
* When 1, {\tt ebreak} instructions in Supervisor Mode enter Halt Mode.
 */
#define CSR_DCSR_EBREAKS_OFFSET             13
#define CSR_DCSR_EBREAKS_LENGTH             1
#define CSR_DCSR_EBREAKS                    (0x1 << CSR_DCSR_EBREAKS_OFFSET)
/*
* When 1, {\tt ebreak} instructions in User/Application Mode enter
* Halt Mode.
 */
#define CSR_DCSR_EBREAKU_OFFSET             12
#define CSR_DCSR_EBREAKU_LENGTH             1
#define CSR_DCSR_EBREAKU                    (0x1 << CSR_DCSR_EBREAKU_OFFSET)
/*
* Controls the behavior of any counters while the component is in
* Halt Mode. This includes the {\tt cycle} and {\tt instret} CSRs.
* When 1, counters are stopped when the component is in Halt Mode.
* Otherwise, the counters continue to run.
*
* An implementation may choose not to support writing to this bit.
* The debugger must read back the value it writes to check whether
* the feature is supported.
 */
#define CSR_DCSR_STOPCYCLE_OFFSET           10
#define CSR_DCSR_STOPCYCLE_LENGTH           1
#define CSR_DCSR_STOPCYCLE                  (0x1 << CSR_DCSR_STOPCYCLE_OFFSET)
/*
* Controls the behavior of any timers while the component is in Debug
* Mode.  This includes the {\tt time} and {tt timeh} CSRs.  When 1,
* timers are stopped when the component is in Halt Mode.  Otherwise,
* the timers continue to run.
*
* An implementation may choose not to support writing to this bit.
* The debugger must read back the value it writes to check whether
* the feature is supported.
 */
#define CSR_DCSR_STOPTIME_OFFSET            9
#define CSR_DCSR_STOPTIME_LENGTH            1
#define CSR_DCSR_STOPTIME                   (0x1 << CSR_DCSR_STOPTIME_OFFSET)
/*
* Explains why Halt Mode was entered.
*
* When there are multiple reasons to enter Halt Mode in a single
* cycle, the cause with the highest priority is the one written.
*
* 1: A software breakpoint was hit. (priority 3)
*
* 2: The Trigger Module caused a halt. (priority 4)
*
* 3: The debug interrupt was asserted by the Debug Module. (priority 2)
*
* 4: The hart single stepped because \Fstep was set. (priority 1)
*
* 5: \Fhaltreq was set. (priority 0)
*
* Other values are reserved for future use.
 */
#define CSR_DCSR_CAUSE_OFFSET               6
#define CSR_DCSR_CAUSE_LENGTH               3
#define CSR_DCSR_CAUSE                      (0x7 << CSR_DCSR_CAUSE_OFFSET)
/*
* When set and not in Halt Mode, the hart will only execute a single
* instruction, and then enter Halt Mode. Interrupts are disabled
* when this bit is set.
 */
#define CSR_DCSR_STEP_OFFSET                2
#define CSR_DCSR_STEP_LENGTH                1
#define CSR_DCSR_STEP                       (0x1 << CSR_DCSR_STEP_OFFSET)
/*
* Contains the privilege level the hart was operating in when Debug
* Mode was entered. The encoding is describe in Table
* \ref{tab:privlevel}.  A debugger can change this value to change
* the hart's privilege level when exiting Halt Mode.
*
* Not all privilege levels are supported on all harts. If the
* encoding written is not supported or the debugger is not allowed to
* change to it, the hart may change to any supported privilege level.
 */
#define CSR_DCSR_PRV_OFFSET                 0
#define CSR_DCSR_PRV_LENGTH                 2
#define CSR_DCSR_PRV                        (0x3 << CSR_DCSR_PRV_OFFSET)
#define CSR_DPC                             0x7b1
#define CSR_DPC_DPC_OFFSET                  0
#define CSR_DPC_DPC_LENGTH                  XLEN
#define CSR_DPC_DPC                         (((1L<<XLEN)-1) << CSR_DPC_DPC_OFFSET)
#define CSR_DSCRATCH0                       0x7b2
#define CSR_DSCRATCH1                       0x7b3
#define CSR_PRIV                            virtual
/*
* Contains the privilege level the hart was operating in when Debug
* Mode was entered. The encoding is describe in Table
* \ref{tab:privlevel}. A user can write this value to change the
* hart's privilege level when exiting Halt Mode.
 */
#define CSR_PRIV_PRV_OFFSET                 0
#define CSR_PRIV_PRV_LENGTH                 2
#define CSR_PRIV_PRV                        (0x3 << CSR_PRIV_PRV_OFFSET)
#define DMI_DMCONTROL                       0x00
/*
* Halt request signal for the hart selected by \Fhartsel. When 1, the
* hart will halt if it's not currently halted.
* Setting both \Fhaltreq and \Fresumereq leads to undefined behavior.
*
* Writes apply to the new value of \Fhartsel.
 */
#define DMI_DMCONTROL_HALTREQ_OFFSET        31
#define DMI_DMCONTROL_HALTREQ_LENGTH        1
#define DMI_DMCONTROL_HALTREQ               (0x1 << DMI_DMCONTROL_HALTREQ_OFFSET)
/*
* Resume request signal for the hart selected by \Fhartsel. When 1,
* the hart will resume if it's currently halted.
* Setting both \Fhaltreq and \Fresumereq leads to undefined behavior.
*
* Writes apply to the new value of \Fhartsel.
 */
#define DMI_DMCONTROL_RESUMEREQ_OFFSET      30
#define DMI_DMCONTROL_RESUMEREQ_LENGTH      1
#define DMI_DMCONTROL_RESUMEREQ             (0x1 << DMI_DMCONTROL_RESUMEREQ_OFFSET)
/*
* The status of the currently selected hart.
*
* 0: Halted.
*
* 1: Running.
*
* 2: Unavailable (eg. powered down, held in reset).
*
* 3: \Fhartsel specifies a hart that does not exist in this system.
 */
#define DMI_DMCONTROL_HARTSTATUS_OFFSET     26
#define DMI_DMCONTROL_HARTSTATUS_LENGTH     2
#define DMI_DMCONTROL_HARTSTATUS            (0x3 << DMI_DMCONTROL_HARTSTATUS_OFFSET)
/*
* The DM-specific index of the hart to select.
 */
#define DMI_DMCONTROL_HARTSEL_OFFSET        16
#define DMI_DMCONTROL_HARTSEL_LENGTH        10
#define DMI_DMCONTROL_HARTSEL               (0x3ff << DMI_DMCONTROL_HARTSEL_OFFSET)
/*
* This bit serves as a reset signal for the Debug Module itself.
* When 0, the module is held in reset. When 1, it functions normally.
* No other mechanism should exist that may result in resetting the
* Debug Module after power up, including the platform's system reset
* or Debug Transport reset signals.
*
* A debugger should pulse this bit low to ensure that the Debug
* Module is fully reset and ready to use.
*
* Implementations may use this bit to aid debugging, for example by
* preventing the Debug Module from being power gated while debugging
* is active.
 */
#define DMI_DMCONTROL_DMACTIVE_OFFSET       9
#define DMI_DMCONTROL_DMACTIVE_LENGTH       1
#define DMI_DMCONTROL_DMACTIVE              (0x1 << DMI_DMCONTROL_DMACTIVE_OFFSET)
/*
* This bit controls the reset signal from the DM to the rest of the
* system. To perform a reset the debugger writes 1, and then writes 0
* to deassert the reset.
 */
#define DMI_DMCONTROL_RESET_OFFSET          8
#define DMI_DMCONTROL_RESET_LENGTH          1
#define DMI_DMCONTROL_RESET                 (0x1 << DMI_DMCONTROL_RESET_OFFSET)
/*
* 0 when authentication is required before using the DM.  1 when the
* authentication check has passed. On components that don't implement
* authentication, this bit must be preset as 1.
 */
#define DMI_DMCONTROL_AUTHENTICATED_OFFSET  7
#define DMI_DMCONTROL_AUTHENTICATED_LENGTH  1
#define DMI_DMCONTROL_AUTHENTICATED         (0x1 << DMI_DMCONTROL_AUTHENTICATED_OFFSET)
/*
* While 1, writes to \Rauthdatazero and \Rauthdataone may be ignored
* or may result in authentication failing.  Authentication mechanisms
* that are slow (or intentionally delayed) must set this bit when
* they're not ready to process another write.
 */
#define DMI_DMCONTROL_AUTHBUSY_OFFSET       6
#define DMI_DMCONTROL_AUTHBUSY_LENGTH       1
#define DMI_DMCONTROL_AUTHBUSY              (0x1 << DMI_DMCONTROL_AUTHBUSY_OFFSET)
/*
* Defines the kind of authentication required to use this DM.
*
* 0: No authentication is required.
*
* 1: A password is required.
*
* 2: A challenge-response mechanism is in place.
*
* 3: Reserved for future use.
 */
#define DMI_DMCONTROL_AUTHTYPE_OFFSET       4
#define DMI_DMCONTROL_AUTHTYPE_LENGTH       2
#define DMI_DMCONTROL_AUTHTYPE              (0x3 << DMI_DMCONTROL_AUTHTYPE_OFFSET)
/*
* 0: There is no Debug Module present.
*
* 1: There is a Debug Module and it conforms to version 0.12 of this
* specification.
*
* Other values are reserved for future use.
 */
#define DMI_DMCONTROL_VERSION_OFFSET        0
#define DMI_DMCONTROL_VERSION_LENGTH        4
#define DMI_DMCONTROL_VERSION               (0xf << DMI_DMCONTROL_VERSION_OFFSET)
#define DMI_HARTINFO                        0x01
/*
* 0: The {\tt data} registers are shadowed in the hart by CSR
* registers. Each CSR register is XLEN bits in size, and corresponds
* to a single argument, per Table~\ref{tab:datareg}.
*
* 1: The {\tt data} registers are shadowed in the hart's memory map.
* Each register takes up 4 bytes in the memory map.
 */
#define DMI_HARTINFO_DATAACCESS_OFFSET      16
#define DMI_HARTINFO_DATAACCESS_LENGTH      1
#define DMI_HARTINFO_DATAACCESS             (0x1 << DMI_HARTINFO_DATAACCESS_OFFSET)
/*
* If \Fdataaccess is 0: Number of CSR registers dedicated to
* shadowing the {\tt data} registers.
*
* If \Fdataaccess is 1: Number of 32-bit words in the memory map
* dedicated to shadowing the {\tt data} registers.
 */
#define DMI_HARTINFO_DATASIZE_OFFSET        12
#define DMI_HARTINFO_DATASIZE_LENGTH        4
#define DMI_HARTINFO_DATASIZE               (0xf << DMI_HARTINFO_DATASIZE_OFFSET)
/*
* If \Fdataaccess is 0: The number of the first CSR dedicated to
* shadowing the {\tt data} registers.
*
* If \Fdataaccess is 1: Signed address of RAM where the {\tt data}
* registers are shadowed.
 */
#define DMI_HARTINFO_DATAADDR_OFFSET        0
#define DMI_HARTINFO_DATAADDR_LENGTH        12
#define DMI_HARTINFO_DATAADDR               (0xfff << DMI_HARTINFO_DATAADDR_OFFSET)
#define DMI_HALTSUM                         0x02
#define DMI_HALTSUM_HALT1023_992_OFFSET     31
#define DMI_HALTSUM_HALT1023_992_LENGTH     1
#define DMI_HALTSUM_HALT1023_992            (0x1 << DMI_HALTSUM_HALT1023_992_OFFSET)
#define DMI_HALTSUM_HALT991_960_OFFSET      30
#define DMI_HALTSUM_HALT991_960_LENGTH      1
#define DMI_HALTSUM_HALT991_960             (0x1 << DMI_HALTSUM_HALT991_960_OFFSET)
#define DMI_HALTSUM_HALT959_928_OFFSET      29
#define DMI_HALTSUM_HALT959_928_LENGTH      1
#define DMI_HALTSUM_HALT959_928             (0x1 << DMI_HALTSUM_HALT959_928_OFFSET)
#define DMI_HALTSUM_HALT927_896_OFFSET      28
#define DMI_HALTSUM_HALT927_896_LENGTH      1
#define DMI_HALTSUM_HALT927_896             (0x1 << DMI_HALTSUM_HALT927_896_OFFSET)
#define DMI_HALTSUM_HALT895_864_OFFSET      27
#define DMI_HALTSUM_HALT895_864_LENGTH      1
#define DMI_HALTSUM_HALT895_864             (0x1 << DMI_HALTSUM_HALT895_864_OFFSET)
#define DMI_HALTSUM_HALT863_832_OFFSET      26
#define DMI_HALTSUM_HALT863_832_LENGTH      1
#define DMI_HALTSUM_HALT863_832             (0x1 << DMI_HALTSUM_HALT863_832_OFFSET)
#define DMI_HALTSUM_HALT831_800_OFFSET      25
#define DMI_HALTSUM_HALT831_800_LENGTH      1
#define DMI_HALTSUM_HALT831_800             (0x1 << DMI_HALTSUM_HALT831_800_OFFSET)
#define DMI_HALTSUM_HALT799_768_OFFSET      24
#define DMI_HALTSUM_HALT799_768_LENGTH      1
#define DMI_HALTSUM_HALT799_768             (0x1 << DMI_HALTSUM_HALT799_768_OFFSET)
#define DMI_HALTSUM_HALT767_736_OFFSET      23
#define DMI_HALTSUM_HALT767_736_LENGTH      1
#define DMI_HALTSUM_HALT767_736             (0x1 << DMI_HALTSUM_HALT767_736_OFFSET)
#define DMI_HALTSUM_HALT735_704_OFFSET      22
#define DMI_HALTSUM_HALT735_704_LENGTH      1
#define DMI_HALTSUM_HALT735_704             (0x1 << DMI_HALTSUM_HALT735_704_OFFSET)
#define DMI_HALTSUM_HALT703_672_OFFSET      21
#define DMI_HALTSUM_HALT703_672_LENGTH      1
#define DMI_HALTSUM_HALT703_672             (0x1 << DMI_HALTSUM_HALT703_672_OFFSET)
#define DMI_HALTSUM_HALT671_640_OFFSET      20
#define DMI_HALTSUM_HALT671_640_LENGTH      1
#define DMI_HALTSUM_HALT671_640             (0x1 << DMI_HALTSUM_HALT671_640_OFFSET)
#define DMI_HALTSUM_HALT639_608_OFFSET      19
#define DMI_HALTSUM_HALT639_608_LENGTH      1
#define DMI_HALTSUM_HALT639_608             (0x1 << DMI_HALTSUM_HALT639_608_OFFSET)
#define DMI_HALTSUM_HALT607_576_OFFSET      18
#define DMI_HALTSUM_HALT607_576_LENGTH      1
#define DMI_HALTSUM_HALT607_576             (0x1 << DMI_HALTSUM_HALT607_576_OFFSET)
#define DMI_HALTSUM_HALT575_544_OFFSET      17
#define DMI_HALTSUM_HALT575_544_LENGTH      1
#define DMI_HALTSUM_HALT575_544             (0x1 << DMI_HALTSUM_HALT575_544_OFFSET)
#define DMI_HALTSUM_HALT543_512_OFFSET      16
#define DMI_HALTSUM_HALT543_512_LENGTH      1
#define DMI_HALTSUM_HALT543_512             (0x1 << DMI_HALTSUM_HALT543_512_OFFSET)
#define DMI_HALTSUM_HALT511_480_OFFSET      15
#define DMI_HALTSUM_HALT511_480_LENGTH      1
#define DMI_HALTSUM_HALT511_480             (0x1 << DMI_HALTSUM_HALT511_480_OFFSET)
#define DMI_HALTSUM_HALT479_448_OFFSET      14
#define DMI_HALTSUM_HALT479_448_LENGTH      1
#define DMI_HALTSUM_HALT479_448             (0x1 << DMI_HALTSUM_HALT479_448_OFFSET)
#define DMI_HALTSUM_HALT447_416_OFFSET      13
#define DMI_HALTSUM_HALT447_416_LENGTH      1
#define DMI_HALTSUM_HALT447_416             (0x1 << DMI_HALTSUM_HALT447_416_OFFSET)
#define DMI_HALTSUM_HALT415_384_OFFSET      12
#define DMI_HALTSUM_HALT415_384_LENGTH      1
#define DMI_HALTSUM_HALT415_384             (0x1 << DMI_HALTSUM_HALT415_384_OFFSET)
#define DMI_HALTSUM_HALT383_352_OFFSET      11
#define DMI_HALTSUM_HALT383_352_LENGTH      1
#define DMI_HALTSUM_HALT383_352             (0x1 << DMI_HALTSUM_HALT383_352_OFFSET)
#define DMI_HALTSUM_HALT351_320_OFFSET      10
#define DMI_HALTSUM_HALT351_320_LENGTH      1
#define DMI_HALTSUM_HALT351_320             (0x1 << DMI_HALTSUM_HALT351_320_OFFSET)
#define DMI_HALTSUM_HALT319_288_OFFSET      9
#define DMI_HALTSUM_HALT319_288_LENGTH      1
#define DMI_HALTSUM_HALT319_288             (0x1 << DMI_HALTSUM_HALT319_288_OFFSET)
#define DMI_HALTSUM_HALT287_256_OFFSET      8
#define DMI_HALTSUM_HALT287_256_LENGTH      1
#define DMI_HALTSUM_HALT287_256             (0x1 << DMI_HALTSUM_HALT287_256_OFFSET)
#define DMI_HALTSUM_HALT255_224_OFFSET      7
#define DMI_HALTSUM_HALT255_224_LENGTH      1
#define DMI_HALTSUM_HALT255_224             (0x1 << DMI_HALTSUM_HALT255_224_OFFSET)
#define DMI_HALTSUM_HALT223_192_OFFSET      6
#define DMI_HALTSUM_HALT223_192_LENGTH      1
#define DMI_HALTSUM_HALT223_192             (0x1 << DMI_HALTSUM_HALT223_192_OFFSET)
#define DMI_HALTSUM_HALT191_160_OFFSET      5
#define DMI_HALTSUM_HALT191_160_LENGTH      1
#define DMI_HALTSUM_HALT191_160             (0x1 << DMI_HALTSUM_HALT191_160_OFFSET)
#define DMI_HALTSUM_HALT159_128_OFFSET      4
#define DMI_HALTSUM_HALT159_128_LENGTH      1
#define DMI_HALTSUM_HALT159_128             (0x1 << DMI_HALTSUM_HALT159_128_OFFSET)
#define DMI_HALTSUM_HALT127_96_OFFSET       3
#define DMI_HALTSUM_HALT127_96_LENGTH       1
#define DMI_HALTSUM_HALT127_96              (0x1 << DMI_HALTSUM_HALT127_96_OFFSET)
#define DMI_HALTSUM_HALT95_64_OFFSET        2
#define DMI_HALTSUM_HALT95_64_LENGTH        1
#define DMI_HALTSUM_HALT95_64               (0x1 << DMI_HALTSUM_HALT95_64_OFFSET)
#define DMI_HALTSUM_HALT63_32_OFFSET        1
#define DMI_HALTSUM_HALT63_32_LENGTH        1
#define DMI_HALTSUM_HALT63_32               (0x1 << DMI_HALTSUM_HALT63_32_OFFSET)
#define DMI_HALTSUM_HALT31_0_OFFSET         0
#define DMI_HALTSUM_HALT31_0_LENGTH         1
#define DMI_HALTSUM_HALT31_0                (0x1 << DMI_HALTSUM_HALT31_0_OFFSET)
#define DMI_SBCS                            0x03
/*
* When a 1 is written here, triggers a read at the address in {\tt
* sbaddress} using the access size set by \Fsbaccess.
 */
#define DMI_SBCS_SBSINGLEREAD_OFFSET        20
#define DMI_SBCS_SBSINGLEREAD_LENGTH        1
#define DMI_SBCS_SBSINGLEREAD               (0x1 << DMI_SBCS_SBSINGLEREAD_OFFSET)
/*
* Select the access size to use for system bus accesses triggered by
* writes to the {\tt sbaddress} registers or \Rsbdatazero.
*
* 0: 8-bit
*
* 1: 16-bit
*
* 2: 32-bit
*
* 3: 64-bit
*
* 4: 128-bit
*
* If an unsupported system bus access size is written here,
* the DM may not
* perform the access, or may perform the access with any access size
 */
#define DMI_SBCS_SBACCESS_OFFSET            17
#define DMI_SBCS_SBACCESS_LENGTH            3
#define DMI_SBCS_SBACCESS                   (0x7 << DMI_SBCS_SBACCESS_OFFSET)
/*
* When 1, the internal address value (used by the system bus master)
* is incremented by the access size (in bytes) selected in \Fsbaccess
* after every system bus access.
 */
#define DMI_SBCS_SBAUTOINCREMENT_OFFSET     16
#define DMI_SBCS_SBAUTOINCREMENT_LENGTH     1
#define DMI_SBCS_SBAUTOINCREMENT            (0x1 << DMI_SBCS_SBAUTOINCREMENT_OFFSET)
/*
* When 1, every read from \Rsbdatazero automatically triggers a system
* bus read at the new address.
 */
#define DMI_SBCS_SBAUTOREAD_OFFSET          15
#define DMI_SBCS_SBAUTOREAD_LENGTH          1
#define DMI_SBCS_SBAUTOREAD                 (0x1 << DMI_SBCS_SBAUTOREAD_OFFSET)
/*
* When the debug module's system bus
* master causes a bus error, this field gets set.
* It remains set until 0 is written to any bit in this field. Until
* that happens, the system bus master is busy and no more accesses can be
* initiated by the debug module.
*
* 0: There was no bus error.
*
* 1: There was a timeout.
*
* 2: A bad address was accessed.
*
* 3: There was some other error (eg. alignment).
*
* 4: The system bus master was busy when a one of the
* {\tt sbaddress} or {\tt sbdata} registers was written,
* or the {\tt sbdata} register was read when it had
* stale data.
 */
#define DMI_SBCS_SBERROR_OFFSET             12
#define DMI_SBCS_SBERROR_LENGTH             3
#define DMI_SBCS_SBERROR                    (0x7 << DMI_SBCS_SBERROR_OFFSET)
/*
* Width of system bus addresses in bits. (0 indicates there is no bus
* access support.)
 */
#define DMI_SBCS_SBASIZE_OFFSET             5
#define DMI_SBCS_SBASIZE_LENGTH             7
#define DMI_SBCS_SBASIZE                    (0x7f << DMI_SBCS_SBASIZE_OFFSET)
/*
* 1 when 128-bit system bus accesses are supported.
 */
#define DMI_SBCS_SBACCESS128_OFFSET         4
#define DMI_SBCS_SBACCESS128_LENGTH         1
#define DMI_SBCS_SBACCESS128                (0x1 << DMI_SBCS_SBACCESS128_OFFSET)
/*
* 1 when 64-bit system bus accesses are supported.
 */
#define DMI_SBCS_SBACCESS64_OFFSET          3
#define DMI_SBCS_SBACCESS64_LENGTH          1
#define DMI_SBCS_SBACCESS64                 (0x1 << DMI_SBCS_SBACCESS64_OFFSET)
/*
* 1 when 32-bit system bus accesses are supported.
 */
#define DMI_SBCS_SBACCESS32_OFFSET          2
#define DMI_SBCS_SBACCESS32_LENGTH          1
#define DMI_SBCS_SBACCESS32                 (0x1 << DMI_SBCS_SBACCESS32_OFFSET)
/*
* 1 when 16-bit system bus accesses are supported.
 */
#define DMI_SBCS_SBACCESS16_OFFSET          1
#define DMI_SBCS_SBACCESS16_LENGTH          1
#define DMI_SBCS_SBACCESS16                 (0x1 << DMI_SBCS_SBACCESS16_OFFSET)
/*
* 1 when 8-bit system bus accesses are supported.
 */
#define DMI_SBCS_SBACCESS8_OFFSET           0
#define DMI_SBCS_SBACCESS8_LENGTH           1
#define DMI_SBCS_SBACCESS8                  (0x1 << DMI_SBCS_SBACCESS8_OFFSET)
#define DMI_SBADDRESS0                      0x04
/*
* Accesses bits 31:0 of the internal address.
 */
#define DMI_SBADDRESS0_ADDRESS_OFFSET       0
#define DMI_SBADDRESS0_ADDRESS_LENGTH       32
#define DMI_SBADDRESS0_ADDRESS              (0xffffffff << DMI_SBADDRESS0_ADDRESS_OFFSET)
#define DMI_SBADDRESS1                      0x05
/*
* Accesses bits 63:32 of the internal address (if the system address
* bus is that wide).
 */
#define DMI_SBADDRESS1_ADDRESS_OFFSET       0
#define DMI_SBADDRESS1_ADDRESS_LENGTH       32
#define DMI_SBADDRESS1_ADDRESS              (0xffffffff << DMI_SBADDRESS1_ADDRESS_OFFSET)
#define DMI_SBADDRESS2                      0x06
/*
* The same as \Fbusy in \Rsbaddresszero.
 */
#define DMI_SBADDRESS2_BUSY_OFFSET          31
#define DMI_SBADDRESS2_BUSY_LENGTH          1
#define DMI_SBADDRESS2_BUSY                 (0x1 << DMI_SBADDRESS2_BUSY_OFFSET)
/*
* Accesses bits 91:61 of the internal address (if the system address
* bus is that wide).
 */
#define DMI_SBADDRESS2_ADDRESS_OFFSET       0
#define DMI_SBADDRESS2_ADDRESS_LENGTH       31
#define DMI_SBADDRESS2_ADDRESS              (0x7fffffff << DMI_SBADDRESS2_ADDRESS_OFFSET)
#define DMI_SBDATA0                         0x07
/*
* Accesses bits 31:0 of the internal data.
 */
#define DMI_SBDATA0_DATA_OFFSET             0
#define DMI_SBDATA0_DATA_LENGTH             32
#define DMI_SBDATA0_DATA                    (0xffffffff << DMI_SBDATA0_DATA_OFFSET)
#define DMI_SBDATA1                         0x08
/*
* Accesses bits 63:32 of the internal data (if the system bus is
* that wide).
 */
#define DMI_SBDATA1_DATA_OFFSET             0
#define DMI_SBDATA1_DATA_LENGTH             32
#define DMI_SBDATA1_DATA                    (0xffffffff << DMI_SBDATA1_DATA_OFFSET)
#define DMI_SBDATA2                         0x09
/*
* Accesses bits 95:64 of the internal data (if the system bus is
* that wide).
 */
#define DMI_SBDATA2_DATA_OFFSET             0
#define DMI_SBDATA2_DATA_LENGTH             32
#define DMI_SBDATA2_DATA                    (0xffffffff << DMI_SBDATA2_DATA_OFFSET)
#define DMI_SBDATA3                         0x0a
/*
* Accesses bits 127:96 of the internal data (if the system bus is
* that wide).
 */
#define DMI_SBDATA3_DATA_OFFSET             0
#define DMI_SBDATA3_DATA_LENGTH             32
#define DMI_SBDATA3_DATA                    (0xffffffff << DMI_SBDATA3_DATA_OFFSET)
#define DMI_AUTHDATA0                       0x0b
#define DMI_AUTHDATA0_DATA_OFFSET           0
#define DMI_AUTHDATA0_DATA_LENGTH           32
#define DMI_AUTHDATA0_DATA                  (0xffffffff << DMI_AUTHDATA0_DATA_OFFSET)
#define DMI_AUTHDATA1                       0x0c
#define DMI_AUTHDATA1_DATA_OFFSET           0
#define DMI_AUTHDATA1_DATA_LENGTH           32
#define DMI_AUTHDATA1_DATA                  (0xffffffff << DMI_AUTHDATA1_DATA_OFFSET)
#define DMI_ABSTRACTCS                      0x0e
#define DMI_ABSTRACTCS_AUTOEXEC7_OFFSET     15
#define DMI_ABSTRACTCS_AUTOEXEC7_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC7            (0x1 << DMI_ABSTRACTCS_AUTOEXEC7_OFFSET)
#define DMI_ABSTRACTCS_AUTOEXEC6_OFFSET     14
#define DMI_ABSTRACTCS_AUTOEXEC6_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC6            (0x1 << DMI_ABSTRACTCS_AUTOEXEC6_OFFSET)
#define DMI_ABSTRACTCS_AUTOEXEC5_OFFSET     13
#define DMI_ABSTRACTCS_AUTOEXEC5_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC5            (0x1 << DMI_ABSTRACTCS_AUTOEXEC5_OFFSET)
#define DMI_ABSTRACTCS_AUTOEXEC4_OFFSET     12
#define DMI_ABSTRACTCS_AUTOEXEC4_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC4            (0x1 << DMI_ABSTRACTCS_AUTOEXEC4_OFFSET)
#define DMI_ABSTRACTCS_AUTOEXEC3_OFFSET     11
#define DMI_ABSTRACTCS_AUTOEXEC3_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC3            (0x1 << DMI_ABSTRACTCS_AUTOEXEC3_OFFSET)
#define DMI_ABSTRACTCS_AUTOEXEC2_OFFSET     10
#define DMI_ABSTRACTCS_AUTOEXEC2_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC2            (0x1 << DMI_ABSTRACTCS_AUTOEXEC2_OFFSET)
#define DMI_ABSTRACTCS_AUTOEXEC1_OFFSET     9
#define DMI_ABSTRACTCS_AUTOEXEC1_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC1            (0x1 << DMI_ABSTRACTCS_AUTOEXEC1_OFFSET)
/*
* When 1, accesses to \Rdatazero cause the command in \Rcommand to be
* executed again.
*
* The same is true for other other autoexec bits: When 1, accesses to
* {\tt data}N cause the command in \Rcommand to be executed again.
 */
#define DMI_ABSTRACTCS_AUTOEXEC0_OFFSET     8
#define DMI_ABSTRACTCS_AUTOEXEC0_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC0            (0x1 << DMI_ABSTRACTCS_AUTOEXEC0_OFFSET)
/*
* Gets set if an abstract command fails. No abstract command is
* started until the value is reset to 0.
*
* 0 (none): No error.
*
* 1 (busy): An abstract command was executing while \Rcommand or one
* of the {\tt data} registers was accessed.
*
* 2 (not supported): The requested command is not supported. A
* command that is not supported while the hart is running may be
* supported when it is halted.
*
* 3 (exception): An exception occurred while executing the command
* (eg. while executing the Program Buffer).
*
* 4 (halt/resume): An abstract command couldn't execute because the
* hart wasn't in the expected state (running/halted).
*
* 7 (other): The command failed for another reason.
 */
#define DMI_ABSTRACTCS_CMDERR_OFFSET        5
#define DMI_ABSTRACTCS_CMDERR_LENGTH        3
#define DMI_ABSTRACTCS_CMDERR               (0x7 << DMI_ABSTRACTCS_CMDERR_OFFSET)
/*
* 1: An abstract command is currently being executed.
*
* This bit is set as soon as \Rcommand is written, and isn't cleared
* until that command has completed.
 */
#define DMI_ABSTRACTCS_BUSY_OFFSET          4
#define DMI_ABSTRACTCS_BUSY_LENGTH          1
#define DMI_ABSTRACTCS_BUSY                 (0x1 << DMI_ABSTRACTCS_BUSY_OFFSET)
/*
* Number of {\tt data} registers that are implemented as part of the
* abstract command interface. If it's 0 then no abstract interface is
* implemented at all.
 */
#define DMI_ABSTRACTCS_DATACOUNT_OFFSET     0
#define DMI_ABSTRACTCS_DATACOUNT_LENGTH     4
#define DMI_ABSTRACTCS_DATACOUNT            (0xf << DMI_ABSTRACTCS_DATACOUNT_OFFSET)
#define DMI_COMMAND                         0x0f
#define DMI_COMMAND_COMMAND_OFFSET          0
#define DMI_COMMAND_COMMAND_LENGTH          32
#define DMI_COMMAND_COMMAND                 (0xffffffff << DMI_COMMAND_COMMAND_OFFSET)
#define DMI_DATA0                           0x10
#define DMI_DATA0_DATA_OFFSET               0
#define DMI_DATA0_DATA_LENGTH               32
#define DMI_DATA0_DATA                      (0xffffffff << DMI_DATA0_DATA_OFFSET)
#define DMI_DATA1                           0x11
#define DMI_DATA2                           0x12
#define DMI_DATA3                           0x13
#define DMI_DATA4                           0x14
#define DMI_DATA5                           0x15
#define DMI_DATA6                           0x16
#define DMI_DATA7                           0x17
#define DMI_DATA8                           0x18
#define DMI_DATA9                           0x19
#define DMI_DATA10                          0x1a
#define DMI_DATA11                          0x1b
#define DMI_SERDATA                         0x1c
#define DMI_SERDATA_DATA_OFFSET             0
#define DMI_SERDATA_DATA_LENGTH             32
#define DMI_SERDATA_DATA                    (0xffffffff << DMI_SERDATA_DATA_OFFSET)
#define DMI_SERSTATUS                       0x1d
/*
* Number of supported serial ports.
 */
#define DMI_SERSTATUS_SERIALCOUNT_OFFSET    28
#define DMI_SERSTATUS_SERIALCOUNT_LENGTH    4
#define DMI_SERSTATUS_SERIALCOUNT           (0xf << DMI_SERSTATUS_SERIALCOUNT_OFFSET)
/*
* Select which serial port is accessed by \Rserdata.
 */
#define DMI_SERSTATUS_SERIAL_OFFSET         16
#define DMI_SERSTATUS_SERIAL_LENGTH         3
#define DMI_SERSTATUS_SERIAL                (0x7 << DMI_SERSTATUS_SERIAL_OFFSET)
#define DMI_SERSTATUS_VALID7_OFFSET         15
#define DMI_SERSTATUS_VALID7_LENGTH         1
#define DMI_SERSTATUS_VALID7                (0x1 << DMI_SERSTATUS_VALID7_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW7_OFFSET 14
#define DMI_SERSTATUS_FULL_OVERFLOW7_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW7        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW7_OFFSET)
#define DMI_SERSTATUS_VALID6_OFFSET         13
#define DMI_SERSTATUS_VALID6_LENGTH         1
#define DMI_SERSTATUS_VALID6                (0x1 << DMI_SERSTATUS_VALID6_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW6_OFFSET 12
#define DMI_SERSTATUS_FULL_OVERFLOW6_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW6        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW6_OFFSET)
#define DMI_SERSTATUS_VALID5_OFFSET         11
#define DMI_SERSTATUS_VALID5_LENGTH         1
#define DMI_SERSTATUS_VALID5                (0x1 << DMI_SERSTATUS_VALID5_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW5_OFFSET 10
#define DMI_SERSTATUS_FULL_OVERFLOW5_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW5        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW5_OFFSET)
#define DMI_SERSTATUS_VALID4_OFFSET         9
#define DMI_SERSTATUS_VALID4_LENGTH         1
#define DMI_SERSTATUS_VALID4                (0x1 << DMI_SERSTATUS_VALID4_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW4_OFFSET 8
#define DMI_SERSTATUS_FULL_OVERFLOW4_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW4        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW4_OFFSET)
#define DMI_SERSTATUS_VALID3_OFFSET         7
#define DMI_SERSTATUS_VALID3_LENGTH         1
#define DMI_SERSTATUS_VALID3                (0x1 << DMI_SERSTATUS_VALID3_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW3_OFFSET 6
#define DMI_SERSTATUS_FULL_OVERFLOW3_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW3        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW3_OFFSET)
#define DMI_SERSTATUS_VALID2_OFFSET         5
#define DMI_SERSTATUS_VALID2_LENGTH         1
#define DMI_SERSTATUS_VALID2                (0x1 << DMI_SERSTATUS_VALID2_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW2_OFFSET 4
#define DMI_SERSTATUS_FULL_OVERFLOW2_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW2        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW2_OFFSET)
#define DMI_SERSTATUS_VALID1_OFFSET         3
#define DMI_SERSTATUS_VALID1_LENGTH         1
#define DMI_SERSTATUS_VALID1                (0x1 << DMI_SERSTATUS_VALID1_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW1_OFFSET 2
#define DMI_SERSTATUS_FULL_OVERFLOW1_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW1        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW1_OFFSET)
/*
* 1 when the core-to-debugger queue for serial port 0 is not empty.
 */
#define DMI_SERSTATUS_VALID0_OFFSET         1
#define DMI_SERSTATUS_VALID0_LENGTH         1
#define DMI_SERSTATUS_VALID0                (0x1 << DMI_SERSTATUS_VALID0_OFFSET)
/*
* 1 when the debugger-to-core queue for serial port 0 is either full,
* or has overflowed. Overflow state is sticky, and can be reset by
* writing 0 to this bit.
 */
#define DMI_SERSTATUS_FULL_OVERFLOW0_OFFSET 0
#define DMI_SERSTATUS_FULL_OVERFLOW0_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW0        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW0_OFFSET)
#define DMI_ACCESSCS                        0x1f
/*
* Size of the Program Buffer, in 32-bit words. Valid sizes are 0 - 12.
*
* A debugger must not access any Instruction Buffer locations that
* fall outside the range specified here.
*
* TODO: Explain what can be done with each size of the buffer, to suggest
* why you would want more or less words.
 */
#define DMI_ACCESSCS_PROGSIZE_OFFSET        0
#define DMI_ACCESSCS_PROGSIZE_LENGTH        4
#define DMI_ACCESSCS_PROGSIZE               (0xf << DMI_ACCESSCS_PROGSIZE_OFFSET)
#define DMI_IBUF0                           0x20
#define DMI_IBUF0_DATA_OFFSET               0
#define DMI_IBUF0_DATA_LENGTH               32
#define DMI_IBUF0_DATA                      (0xffffffff << DMI_IBUF0_DATA_OFFSET)
#define DMI_IBUF1                           0x21
#define DMI_IBUF2                           0x22
#define DMI_IBUF3                           0x23
#define DMI_IBUF4                           0x24
#define DMI_IBUF5                           0x25
#define DMI_IBUF6                           0x26
#define DMI_IBUF7                           0x27
#define DMI_IBUF8                           0x28
#define DMI_IBUF9                           0x29
#define DMI_IBUF10                          0x2a
#define DMI_IBUF11                          0x2b
#define SERINFO                             0x110
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL7_OFFSET              7
#define SERINFO_SERIAL7_LENGTH              1
#define SERINFO_SERIAL7                     (0x1 << SERINFO_SERIAL7_OFFSET)
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL6_OFFSET              6
#define SERINFO_SERIAL6_LENGTH              1
#define SERINFO_SERIAL6                     (0x1 << SERINFO_SERIAL6_OFFSET)
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL5_OFFSET              5
#define SERINFO_SERIAL5_LENGTH              1
#define SERINFO_SERIAL5                     (0x1 << SERINFO_SERIAL5_OFFSET)
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL4_OFFSET              4
#define SERINFO_SERIAL4_LENGTH              1
#define SERINFO_SERIAL4                     (0x1 << SERINFO_SERIAL4_OFFSET)
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL3_OFFSET              3
#define SERINFO_SERIAL3_LENGTH              1
#define SERINFO_SERIAL3                     (0x1 << SERINFO_SERIAL3_OFFSET)
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL2_OFFSET              2
#define SERINFO_SERIAL2_LENGTH              1
#define SERINFO_SERIAL2                     (0x1 << SERINFO_SERIAL2_OFFSET)
/*
* Like \Fserialzero.
 */
#define SERINFO_SERIAL1_OFFSET              1
#define SERINFO_SERIAL1_LENGTH              1
#define SERINFO_SERIAL1                     (0x1 << SERINFO_SERIAL1_OFFSET)
/*
* 1 means serial interface 0 is supported.
 */
#define SERINFO_SERIAL0_OFFSET              0
#define SERINFO_SERIAL0_LENGTH              1
#define SERINFO_SERIAL0                     (0x1 << SERINFO_SERIAL0_OFFSET)
#define SERSEND0                            0x200
#define SERRECV0                            0x204
#define SERSTAT0                            0x208
/*
* Send ready. 1 when the core-to-debugger queue is not full. 0
* otherwise.
 */
#define SERSTAT0_SENDR_OFFSET               1
#define SERSTAT0_SENDR_LENGTH               1
#define SERSTAT0_SENDR                      (0x1 << SERSTAT0_SENDR_OFFSET)
/*
* Receive ready. 1 when the debugger-to-core queue is not empty. 0
* otherwise.
 */
#define SERSTAT0_RECVR_OFFSET               0
#define SERSTAT0_RECVR_LENGTH               1
#define SERSTAT0_RECVR                      (0x1 << SERSTAT0_RECVR_OFFSET)
#define SERSEND1                            0x20c
#define SERRECV1                            0x210
#define SERSTAT1                            0x214
#define SERSEND2                            0x218
#define SERRECV2                            0x21c
#define SERSTAT2                            0x220
#define SERSEND3                            0x224
#define SERRECV3                            0x228
#define SERSTAT3                            0x22c
#define SERSEND4                            0x230
#define SERRECV4                            0x234
#define SERSTAT4                            0x238
#define SERSEND5                            0x23c
#define SERRECV5                            0x240
#define SERSTAT5                            0x244
#define SERSEND6                            0x248
#define SERRECV6                            0x24c
#define SERSTAT6                            0x250
#define SERSEND7                            0x254
#define SERRECV7                            0x258
#define SERSTAT7                            0x25c
#define CSR_TSELECT                         0x7a0
#define CSR_TSELECT_INDEX_OFFSET            0
#define CSR_TSELECT_INDEX_LENGTH            XLEN
#define CSR_TSELECT_INDEX                   (((1L<<XLEN)-1) << CSR_TSELECT_INDEX_OFFSET)
#define CSR_TDATA1                          0x7a1
/*
* 0: There is no trigger at this \Rtselect.
*
* 1: The trigger is a legacy SiFive address match trigger. These
* should not be implemented and aren't further documented here.
*
* 2: The trigger is an address/data match trigger.
*
* 3: The trigger is an instruction count trigger.
*
* 15: This trigger exists (so enumeration shouldn't terminate), but
* is not currently available.
*
* Other values are reserved for future use.
 */
#define CSR_TDATA1_TYPE_OFFSET              XLEN-4
#define CSR_TDATA1_TYPE_LENGTH              4
#define CSR_TDATA1_TYPE                     (0xfL << CSR_TDATA1_TYPE_OFFSET)
/*
* 0: Both Debug and M Mode can write the {\tt tdata} registers at the
* selected \Rtselect.
*
* 1: Only Debug Mode can write the {\tt tdata} registers at the
* selected \Rtselect.  Writes from other modes are ignored.
*
* This bit is only writable from Debug Mode.
 */
#define CSR_TDATA1_DMODE_OFFSET             XLEN-5
#define CSR_TDATA1_DMODE_LENGTH             1
#define CSR_TDATA1_DMODE                    (0x1L << CSR_TDATA1_DMODE_OFFSET)
/*
* Trigger-specific data.
 */
#define CSR_TDATA1_DATA_OFFSET              0
#define CSR_TDATA1_DATA_LENGTH              XLEN - 5
#define CSR_TDATA1_DATA                     (((1L<<XLEN - 5)-1) << CSR_TDATA1_DATA_OFFSET)
#define CSR_TDATA2                          0x7a2
#define CSR_TDATA2_DATA_OFFSET              0
#define CSR_TDATA2_DATA_LENGTH              XLEN
#define CSR_TDATA2_DATA                     (((1L<<XLEN)-1) << CSR_TDATA2_DATA_OFFSET)
#define CSR_TDATA3                          0x7a3
#define CSR_TDATA3_DATA_OFFSET              0
#define CSR_TDATA3_DATA_LENGTH              XLEN
#define CSR_TDATA3_DATA                     (((1L<<XLEN)-1) << CSR_TDATA3_DATA_OFFSET)
#define CSR_MCONTROL                        0x7a1
#define CSR_MCONTROL_TYPE_OFFSET            XLEN-4
#define CSR_MCONTROL_TYPE_LENGTH            4
#define CSR_MCONTROL_TYPE                   (0xfL << CSR_MCONTROL_TYPE_OFFSET)
#define CSR_MCONTROL_DMODE_OFFSET           XLEN-5
#define CSR_MCONTROL_DMODE_LENGTH           1
#define CSR_MCONTROL_DMODE                  (0x1L << CSR_MCONTROL_DMODE_OFFSET)
/*
* Specifies the largest naturally aligned powers-of-two (NAPOT) range
* supported by the hardware. The value is the logarithm base 2 of the
* number of bytes in that range.  A value of 0 indicates that only
* exact value matches are supported (one byte range). A value of 63
* corresponds to the maximum NAPOT range, which is $2^{63}$ bytes in
* size.
 */
#define CSR_MCONTROL_MASKMAX_OFFSET         XLEN-11
#define CSR_MCONTROL_MASKMAX_LENGTH         6
#define CSR_MCONTROL_MASKMAX                (0x3fL << CSR_MCONTROL_MASKMAX_OFFSET)
/*
* 0: Perform a match on the address.
*
* 1: Perform a match on the data value loaded/stored, or the
* instruction executed.
 */
#define CSR_MCONTROL_SELECT_OFFSET          19
#define CSR_MCONTROL_SELECT_LENGTH          1
#define CSR_MCONTROL_SELECT                 (0x1L << CSR_MCONTROL_SELECT_OFFSET)
/*
* 0: The action for this trigger will be taken just before the
* instruction that triggered it is executed, but after all preceding
* instructions are are committed.
*
* 1: The action for this trigger will be taken after the instruction
* that triggered it is executed. It should be taken before the next
* instruction is executed, but it is better to implement triggers and
* not implement that suggestion than to not implement them at all.
*
* Most hardware will only implement one timing or the other, possibly
* dependent on \Fselect, \Fexecute, \Fload, and \Fstore. This bit
* primarily exists for the hardware to communicate to the debugger
* what will happen. Hardware may implement the bit fully writable, in
* which case the debugger has a little more control.
*
* Data load triggers with \Ftiming of 0 will result in the same load
* happening again when the debugger lets the core run. For data load
* triggers debuggers must first attempt to set the breakpoint with
* \Ftiming of 1.
*
* A chain of triggers that don't all have the same \Ftiming value
* will never fire (unless consecutive instructions match the
* appropriate triggers).
 */
#define CSR_MCONTROL_TIMING_OFFSET          18
#define CSR_MCONTROL_TIMING_LENGTH          1
#define CSR_MCONTROL_TIMING                 (0x1L << CSR_MCONTROL_TIMING_OFFSET)
/*
* Determines what happens when this trigger matches.
*
* 0: Raise a breakpoint exception. (Used when software wants to use
* the trigger module without an external debugger attached.)
*
* 1: Enter Debug Mode. (Only supported when \Fdmode is 1.)
*
* 2: Start tracing.
*
* 3: Stop tracing.
*
* 4: Emit trace data for this match. If it is a data access match,
* emit appropriate Load/Store Address/Data. If it is an instruction
* execution, emit its PC.
*
* Other values are reserved for future use.
 */
#define CSR_MCONTROL_ACTION_OFFSET          12
#define CSR_MCONTROL_ACTION_LENGTH          6
#define CSR_MCONTROL_ACTION                 (0x3fL << CSR_MCONTROL_ACTION_OFFSET)
/*
* 0: When this trigger matches, the configured action is taken.
*
* 1: While this trigger does not match, it prevents the trigger with
* the next index from matching.
 */
#define CSR_MCONTROL_CHAIN_OFFSET           11
#define CSR_MCONTROL_CHAIN_LENGTH           1
#define CSR_MCONTROL_CHAIN                  (0x1L << CSR_MCONTROL_CHAIN_OFFSET)
/*
* 0: Matches when the value equals \Rtdatatwo.
*
* 1: Matches when the top M bits of the value match the top M bits of
* \Rtdatatwo. M is XLEN-1 minus the index of the least-significant
* bit containing 0 in \Rtdatatwo.
*
* 2: Matches when the value is greater than or equal to \Rtdatatwo.
*
* 3: Matches when the value is less than \Rtdatatwo.
*
* 4: Matches when the lower half of the value equals the lower half
* of \Rtdatatwo after the lower half of the value is ANDed with the
* upper half of \Rtdatatwo.
*
* 5: Matches when the upper half of the value equals the lower half
* of \Rtdatatwo after the upper half of the value is ANDed with the
* upper half of \Rtdatatwo.
*
* Other values are reserved for future use.
 */
#define CSR_MCONTROL_MATCH_OFFSET           7
#define CSR_MCONTROL_MATCH_LENGTH           4
#define CSR_MCONTROL_MATCH                  (0xfL << CSR_MCONTROL_MATCH_OFFSET)
/*
* When set, enable this trigger in M mode.
 */
#define CSR_MCONTROL_M_OFFSET               6
#define CSR_MCONTROL_M_LENGTH               1
#define CSR_MCONTROL_M                      (0x1L << CSR_MCONTROL_M_OFFSET)
/*
* When set, enable this trigger in H mode.
 */
#define CSR_MCONTROL_H_OFFSET               5
#define CSR_MCONTROL_H_LENGTH               1
#define CSR_MCONTROL_H                      (0x1L << CSR_MCONTROL_H_OFFSET)
/*
* When set, enable this trigger in S mode.
 */
#define CSR_MCONTROL_S_OFFSET               4
#define CSR_MCONTROL_S_LENGTH               1
#define CSR_MCONTROL_S                      (0x1L << CSR_MCONTROL_S_OFFSET)
/*
* When set, enable this trigger in U mode.
 */
#define CSR_MCONTROL_U_OFFSET               3
#define CSR_MCONTROL_U_LENGTH               1
#define CSR_MCONTROL_U                      (0x1L << CSR_MCONTROL_U_OFFSET)
/*
* When set, the trigger fires on the address or opcode of an
* instruction that is executed.
 */
#define CSR_MCONTROL_EXECUTE_OFFSET         2
#define CSR_MCONTROL_EXECUTE_LENGTH         1
#define CSR_MCONTROL_EXECUTE                (0x1L << CSR_MCONTROL_EXECUTE_OFFSET)
/*
* When set, the trigger fires on the address or data of a store.
 */
#define CSR_MCONTROL_STORE_OFFSET           1
#define CSR_MCONTROL_STORE_LENGTH           1
#define CSR_MCONTROL_STORE                  (0x1L << CSR_MCONTROL_STORE_OFFSET)
/*
* When set, the trigger fires on the address or data of a load.
 */
#define CSR_MCONTROL_LOAD_OFFSET            0
#define CSR_MCONTROL_LOAD_LENGTH            1
#define CSR_MCONTROL_LOAD                   (0x1L << CSR_MCONTROL_LOAD_OFFSET)
#define CSR_ICOUNT                          0x7a1
#define CSR_ICOUNT_TYPE_OFFSET              XLEN-4
#define CSR_ICOUNT_TYPE_LENGTH              4
#define CSR_ICOUNT_TYPE                     (0xfL << CSR_ICOUNT_TYPE_OFFSET)
#define CSR_ICOUNT_DMODE_OFFSET             XLEN-5
#define CSR_ICOUNT_DMODE_LENGTH             1
#define CSR_ICOUNT_DMODE                    (0x1L << CSR_ICOUNT_DMODE_OFFSET)
/*
* When count is decremented to 0, the trigger fires. Instead of
* changing \Fcount from 1 to 0, it is also acceptable for hardware to
* clear \Fm, \Fh, \Fs, and \Fu. This allows \Fcount to be hard-wired
* to 1 if this register just exists for single step.
 */
#define CSR_ICOUNT_COUNT_OFFSET             10
#define CSR_ICOUNT_COUNT_LENGTH             14
#define CSR_ICOUNT_COUNT                    (0x3fffL << CSR_ICOUNT_COUNT_OFFSET)
/*
* When set, every instruction completed in M mode decrements \Fcount
* by 1.
 */
#define CSR_ICOUNT_M_OFFSET                 9
#define CSR_ICOUNT_M_LENGTH                 1
#define CSR_ICOUNT_M                        (0x1L << CSR_ICOUNT_M_OFFSET)
/*
* When set, every instruction completed in H mode decrements \Fcount
* by 1.
 */
#define CSR_ICOUNT_H_OFFSET                 8
#define CSR_ICOUNT_H_LENGTH                 1
#define CSR_ICOUNT_H                        (0x1L << CSR_ICOUNT_H_OFFSET)
/*
* When set, every instruction completed in S mode decrements \Fcount
* by 1.
 */
#define CSR_ICOUNT_S_OFFSET                 7
#define CSR_ICOUNT_S_LENGTH                 1
#define CSR_ICOUNT_S                        (0x1L << CSR_ICOUNT_S_OFFSET)
/*
* When set, every instruction completed in U mode decrements \Fcount
* by 1.
 */
#define CSR_ICOUNT_U_OFFSET                 6
#define CSR_ICOUNT_U_LENGTH                 1
#define CSR_ICOUNT_U                        (0x1L << CSR_ICOUNT_U_OFFSET)
/*
* Determines what happens when this trigger matches.
*
* 0: Raise a debug exception. (Used when software wants to use the
* trigger module without an external debugger attached.)
*
* 1: Enter Debug Mode. (Only supported when \Fdmode is 1.)
*
* 2: Start tracing.
*
* 3: Stop tracing.
*
* 4: Emit trace data for this match. If it is a data access match,
* emit appropriate Load/Store Address/Data. If it is an instruction
* execution, emit its PC.
*
* Other values are reserved for future use.
 */
#define CSR_ICOUNT_ACTION_OFFSET            0
#define CSR_ICOUNT_ACTION_LENGTH            6
#define CSR_ICOUNT_ACTION                   (0x3fL << CSR_ICOUNT_ACTION_OFFSET)
#define DTM_IDCODE                          0x01
/*
* Identifies the release version of this part.
 */
#define DTM_IDCODE_VERSION_OFFSET           28
#define DTM_IDCODE_VERSION_LENGTH           4
#define DTM_IDCODE_VERSION                  (0xf << DTM_IDCODE_VERSION_OFFSET)
/*
* Identifies the designer's part number of this part.
 */
#define DTM_IDCODE_PARTNUMBER_OFFSET        12
#define DTM_IDCODE_PARTNUMBER_LENGTH        16
#define DTM_IDCODE_PARTNUMBER               (0xffff << DTM_IDCODE_PARTNUMBER_OFFSET)
/*
* Identifies the designer/manufacturer of this part. Bits 6:0 must be
* bits 6:0 of the designer/manufacturer's Identification Code as
* assigned by JEDEC Standard JEP106. Bits 10:7 contain the modulo-16
* count of the number of continuation characters (0x7f) in that same
* Identification Code.
 */
#define DTM_IDCODE_MANUFID_OFFSET           1
#define DTM_IDCODE_MANUFID_LENGTH           11
#define DTM_IDCODE_MANUFID                  (0x7ff << DTM_IDCODE_MANUFID_OFFSET)
#define DTM_IDCODE_1_OFFSET                 0
#define DTM_IDCODE_1_LENGTH                 1
#define DTM_IDCODE_1                        (0x1 << DTM_IDCODE_1_OFFSET)
#define DTM_SAMPLE                          0x02
#define DTM_PRELOAD                         0x03
#define DTM_EXTEST                          0x04
#define DTM_CLAMP                           0x05
#define DTM_CLAMP__HOLD                     0x06
#define DTM_CLAMP__RELEASE                  0x07
#define DTM_HIGHZ                           0x08
#define DTM_IC__RESET                       0x09
#define DTM_TMP__STATUS                     0x0a
#define DTM_INIT__SETUP                     0x0b
#define DTM_INIT__SETUP__CLAMP              0x0c
#define DTM_INIT__RUN                       0x0d
#define DTM_DTMCONTROL                      0x10
/*
* Writing 1 to this bit resets the DMI controller, clearing any
* sticky error state.
 */
#define DTM_DTMCONTROL_DMIRESET_OFFSET      16
#define DTM_DTMCONTROL_DMIRESET_LENGTH      1
#define DTM_DTMCONTROL_DMIRESET             (0x1 << DTM_DTMCONTROL_DMIRESET_OFFSET)
/*
* This is the minimum number of cycles a debugger should spend in
* Run-Test/Idle after every DMI scan to avoid a 'busy'
* return code (\Fdmistat of 3). A debugger must still
* check \Fdmistat when necessary.
*
* 0: It is not necessary to enter Run-Test/Idle at all.
*
* 1: Enter Run-Test/Idle and leave it immediately.
*
* 2: Enter Run-Test/Idle and stay there for 1 cycle before leaving.
*
* And so on.
 */
#define DTM_DTMCONTROL_IDLE_OFFSET          12
#define DTM_DTMCONTROL_IDLE_LENGTH          3
#define DTM_DTMCONTROL_IDLE                 (0x7 << DTM_DTMCONTROL_IDLE_OFFSET)
/*
* 0: No error.
*
* 1: Reserved. Interpret the same as 2.
*
* 2: An operation failed (resulted in \Fop of 2).
*
* 3: An operation was attempted while a DMI access was still in
* progress (resulted in \Fop of 3).
 */
#define DTM_DTMCONTROL_DMISTAT_OFFSET       10
#define DTM_DTMCONTROL_DMISTAT_LENGTH       2
#define DTM_DTMCONTROL_DMISTAT              (0x3 << DTM_DTMCONTROL_DMISTAT_OFFSET)
/*
* The size of \Faddress in \Rdmi.
 */
#define DTM_DTMCONTROL_ABITS_OFFSET         4
#define DTM_DTMCONTROL_ABITS_LENGTH         6
#define DTM_DTMCONTROL_ABITS                (0x3f << DTM_DTMCONTROL_ABITS_OFFSET)
/*
* 0: Version described in spec version 0.11.
*
* 1: Version described in spec version 0.12 (and later?), which
* reduces the DMI data width to 32 bits.
*
* Other values are reserved for future use.
 */
#define DTM_DTMCONTROL_VERSION_OFFSET       0
#define DTM_DTMCONTROL_VERSION_LENGTH       4
#define DTM_DTMCONTROL_VERSION              (0xf << DTM_DTMCONTROL_VERSION_OFFSET)
#define DTM_DMI                             0x11
/*
* Address used for DMI access. In Update-DR this value is used
* to access the DM over the DMI.
 */
#define DTM_DMI_ADDRESS_OFFSET              34
#define DTM_DMI_ADDRESS_LENGTH              abits
#define DTM_DMI_ADDRESS                     (((1L<<abits)-1) << DTM_DMI_ADDRESS_OFFSET)
/*
* The data to send to the DM over the DMI during Update-DR, and
* the data returned from the DM as a result of the previous operation.
 */
#define DTM_DMI_DATA_OFFSET                 2
#define DTM_DMI_DATA_LENGTH                 32
#define DTM_DMI_DATA                        (0xffffffffL << DTM_DMI_DATA_OFFSET)
/*
* When the debugger writes this field, it has the following meaning:
*
* 0: Ignore \Fdata. (nop)
*
* 1: Read from \Faddress. (read)
*
* 2: Write \Fdata to \Faddress. (write)
*
* 3: Reserved.
*
* When the debugger reads this field, it means the following:
*
* 0: The previous operation completed successfully.
*
* 1: Reserved.
*
* 2: The previous operation returned a non-zero value in \Fop.
* The data scanned into \Rdmi in this access will be ignored.
* This status is sticky and can be cleared by writing \Fdmireset
* in \Rdtmcontrol.
*
* (This indicates that the DM itself responded with an error, e.g.
* in the System Bus and Serial Port overflow/underflow cases.
* Generally this means that for this type of DM access, the DTM should
* allow more time between Update-DR and Capture-DR. The most portable way
* to achieve this is to spend more TCK ticks in Run-Test/Idle state
* for similar operations.)
*
* 3: The previous DMI request is still in progress. The data scanned
* into \Rdmi in this access will be ignored. This status is sticky
* and can be cleared by writing \Fdmireset in \Rdtmcontrol. If a
* debugger sees this status, it needs to give the target more TCK
* edges between Update-DR and Capture-DR. The simplest way
* to do that is to add extra transitions in Run-Test/Idle.
*
* (The DTM, DM, and/or component may be in different clock domains,
* so synchronization may be required. Some relatively fixed number of
* TCK ticks may be needed for the request to reach the DM, complete,
* and for the response to be synchronized back into the TCK domain.
* This status is intended to cover these cases, and is orthogonal to
* the causes for case 2.)
 */
#define DTM_DMI_OP_OFFSET                   0
#define DTM_DMI_OP_LENGTH                   2
#define DTM_DMI_OP                          (0x3L << DTM_DMI_OP_OFFSET)
#define SHORTNAME                           0x123
/*
* Description of what this field is used for.
 */
#define SHORTNAME_FIELD_OFFSET              0
#define SHORTNAME_FIELD_LENGTH              8
#define SHORTNAME_FIELD                     (0xff << SHORTNAME_FIELD_OFFSET)
#define TRACE                               0x728
/*
* 1 if the trace buffer has wrapped since the last time \Fdiscard was
* written. 0 otherwise.
 */
#define TRACE_WRAPPED_OFFSET                24
#define TRACE_WRAPPED_LENGTH                1
#define TRACE_WRAPPED                       (0x1 << TRACE_WRAPPED_OFFSET)
/*
* Emit Timestamp trace sequences.
 */
#define TRACE_EMITTIMESTAMP_OFFSET          23
#define TRACE_EMITTIMESTAMP_LENGTH          1
#define TRACE_EMITTIMESTAMP                 (0x1 << TRACE_EMITTIMESTAMP_OFFSET)
/*
* Emit Store Data trace sequences.
 */
#define TRACE_EMITSTOREDATA_OFFSET          22
#define TRACE_EMITSTOREDATA_LENGTH          1
#define TRACE_EMITSTOREDATA                 (0x1 << TRACE_EMITSTOREDATA_OFFSET)
/*
* Emit Load Data trace sequences.
 */
#define TRACE_EMITLOADDATA_OFFSET           21
#define TRACE_EMITLOADDATA_LENGTH           1
#define TRACE_EMITLOADDATA                  (0x1 << TRACE_EMITLOADDATA_OFFSET)
/*
* Emit Store Address trace sequences.
 */
#define TRACE_EMITSTOREADDR_OFFSET          20
#define TRACE_EMITSTOREADDR_LENGTH          1
#define TRACE_EMITSTOREADDR                 (0x1 << TRACE_EMITSTOREADDR_OFFSET)
/*
* Emit Load Address trace sequences.
 */
#define TRACE_EMITLOADADDR_OFFSET           19
#define TRACE_EMITLOADADDR_LENGTH           1
#define TRACE_EMITLOADADDR                  (0x1 << TRACE_EMITLOADADDR_OFFSET)
/*
* Emit Privilege Level trace sequences.
 */
#define TRACE_EMITPRIV_OFFSET               18
#define TRACE_EMITPRIV_LENGTH               1
#define TRACE_EMITPRIV                      (0x1 << TRACE_EMITPRIV_OFFSET)
/*
* Emit Branch Taken and Branch Not Taken trace sequences.
 */
#define TRACE_EMITBRANCH_OFFSET             17
#define TRACE_EMITBRANCH_LENGTH             1
#define TRACE_EMITBRANCH                    (0x1 << TRACE_EMITBRANCH_OFFSET)
/*
* Emit PC trace sequences.
 */
#define TRACE_EMITPC_OFFSET                 16
#define TRACE_EMITPC_LENGTH                 1
#define TRACE_EMITPC                        (0x1 << TRACE_EMITPC_OFFSET)
/*
* Determine what happens when the trace buffer is full.  0 means wrap
* and overwrite. 1 means turn off trace until \Fdiscard is written as 1.
* 2 means cause a trace full exception. 3 is reserved for future use.
 */
#define TRACE_FULLACTION_OFFSET             8
#define TRACE_FULLACTION_LENGTH             2
#define TRACE_FULLACTION                    (0x3 << TRACE_FULLACTION_OFFSET)
/*
* 0: Trace to a dedicated on-core RAM (which is not further defined in
* this spec).
*
* 1: Trace to RAM on the system bus.
*
* 2: Send trace data to a dedicated off-chip interface (which is not
* defined in this spec). This does not affect execution speed.
*
* 3: Reserved for future use.
*
* Options 0 and 1 slow down execution (eg. because of system bus
* contention).
 */
#define TRACE_DESTINATION_OFFSET            4
#define TRACE_DESTINATION_LENGTH            2
#define TRACE_DESTINATION                   (0x3 << TRACE_DESTINATION_OFFSET)
/*
* When 1, the trace logic may stall processor execution to ensure it
* can emit all the trace sequences required. When 0 individual trace
* sequences may be dropped.
 */
#define TRACE_STALL_OFFSET                  2
#define TRACE_STALL_LENGTH                  1
#define TRACE_STALL                         (0x1 << TRACE_STALL_OFFSET)
/*
* Writing 1 to this bit tells the trace logic that any trace
* collected is no longer required. When tracing to RAM, it resets the
* trace write pointer to the start of the memory, as well as
* \Fwrapped.
 */
#define TRACE_DISCARD_OFFSET                1
#define TRACE_DISCARD_LENGTH                1
#define TRACE_DISCARD                       (0x1 << TRACE_DISCARD_OFFSET)
#define TRACE_SUPPORTED_OFFSET              0
#define TRACE_SUPPORTED_LENGTH              1
#define TRACE_SUPPORTED                     (0x1 << TRACE_SUPPORTED_OFFSET)
#define TBUFSTART                           0x729
#define TBUFEND                             0x72a
#define TBUFWRITE                           0x72b
