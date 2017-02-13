#define AC_ACCESS_REGISTER                  None
#define AC_ACCESS_REGISTER_SIZE_OFFSET      19
#define AC_ACCESS_REGISTER_SIZE_LENGTH      3
#define AC_ACCESS_REGISTER_SIZE             (0x7 << AC_ACCESS_REGISTER_SIZE_OFFSET)
#define AC_ACCESS_REGISTER_PREEXEC_OFFSET   18
#define AC_ACCESS_REGISTER_PREEXEC_LENGTH   1
#define AC_ACCESS_REGISTER_PREEXEC          (0x1 << AC_ACCESS_REGISTER_PREEXEC_OFFSET)
#define AC_ACCESS_REGISTER_POSTEXEC_OFFSET  17
#define AC_ACCESS_REGISTER_POSTEXEC_LENGTH  1
#define AC_ACCESS_REGISTER_POSTEXEC         (0x1 << AC_ACCESS_REGISTER_POSTEXEC_OFFSET)
#define AC_ACCESS_REGISTER_WRITE_OFFSET     16
#define AC_ACCESS_REGISTER_WRITE_LENGTH     1
#define AC_ACCESS_REGISTER_WRITE            (0x1 << AC_ACCESS_REGISTER_WRITE_OFFSET)
#define AC_ACCESS_REGISTER_REGNO_OFFSET     0
#define AC_ACCESS_REGISTER_REGNO_LENGTH     16
#define AC_ACCESS_REGISTER_REGNO            (0xffff << AC_ACCESS_REGISTER_REGNO_OFFSET)
#define AC_QUICK_ACCESS                     None
#define AC_QUICK_ACCESS_1_OFFSET            24
#define AC_QUICK_ACCESS_1_LENGTH            8
#define AC_QUICK_ACCESS_1                   (0xff << AC_QUICK_ACCESS_1_OFFSET)
#define CSR_DCSR                            0x7b0
#define CSR_DCSR_XDEBUGVER_OFFSET           30
#define CSR_DCSR_XDEBUGVER_LENGTH           2
#define CSR_DCSR_XDEBUGVER                  (0x3 << CSR_DCSR_XDEBUGVER_OFFSET)
#define CSR_DCSR_EBREAKM_OFFSET             15
#define CSR_DCSR_EBREAKM_LENGTH             1
#define CSR_DCSR_EBREAKM                    (0x1 << CSR_DCSR_EBREAKM_OFFSET)
#define CSR_DCSR_EBREAKH_OFFSET             14
#define CSR_DCSR_EBREAKH_LENGTH             1
#define CSR_DCSR_EBREAKH                    (0x1 << CSR_DCSR_EBREAKH_OFFSET)
#define CSR_DCSR_EBREAKS_OFFSET             13
#define CSR_DCSR_EBREAKS_LENGTH             1
#define CSR_DCSR_EBREAKS                    (0x1 << CSR_DCSR_EBREAKS_OFFSET)
#define CSR_DCSR_EBREAKU_OFFSET             12
#define CSR_DCSR_EBREAKU_LENGTH             1
#define CSR_DCSR_EBREAKU                    (0x1 << CSR_DCSR_EBREAKU_OFFSET)
#define CSR_DCSR_STOPCYCLE_OFFSET           10
#define CSR_DCSR_STOPCYCLE_LENGTH           1
#define CSR_DCSR_STOPCYCLE                  (0x1 << CSR_DCSR_STOPCYCLE_OFFSET)
#define CSR_DCSR_STOPTIME_OFFSET            9
#define CSR_DCSR_STOPTIME_LENGTH            1
#define CSR_DCSR_STOPTIME                   (0x1 << CSR_DCSR_STOPTIME_OFFSET)
#define CSR_DCSR_CAUSE_OFFSET               6
#define CSR_DCSR_CAUSE_LENGTH               3
#define CSR_DCSR_CAUSE                      (0x7 << CSR_DCSR_CAUSE_OFFSET)
#define CSR_DCSR_STEP_OFFSET                2
#define CSR_DCSR_STEP_LENGTH                1
#define CSR_DCSR_STEP                       (0x1 << CSR_DCSR_STEP_OFFSET)
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
#define CSR_PRIV_PRV_OFFSET                 0
#define CSR_PRIV_PRV_LENGTH                 2
#define CSR_PRIV_PRV                        (0x3 << CSR_PRIV_PRV_OFFSET)
#define DMI_DMCONTROL                       0x00
#define DMI_DMCONTROL_HALTREQ_OFFSET        31
#define DMI_DMCONTROL_HALTREQ_LENGTH        1
#define DMI_DMCONTROL_HALTREQ               (0x1 << DMI_DMCONTROL_HALTREQ_OFFSET)
#define DMI_DMCONTROL_RESET_OFFSET          30
#define DMI_DMCONTROL_RESET_LENGTH          1
#define DMI_DMCONTROL_RESET                 (0x1 << DMI_DMCONTROL_RESET_OFFSET)
#define DMI_DMCONTROL_DMACTIVE_OFFSET       29
#define DMI_DMCONTROL_DMACTIVE_LENGTH       1
#define DMI_DMCONTROL_DMACTIVE              (0x1 << DMI_DMCONTROL_DMACTIVE_OFFSET)
#define DMI_DMCONTROL_HARTSTATUS_OFFSET     26
#define DMI_DMCONTROL_HARTSTATUS_LENGTH     2
#define DMI_DMCONTROL_HARTSTATUS            (0x3 << DMI_DMCONTROL_HARTSTATUS_OFFSET)
#define DMI_DMCONTROL_HARTSEL_OFFSET        16
#define DMI_DMCONTROL_HARTSEL_LENGTH        10
#define DMI_DMCONTROL_HARTSEL               (0x3ff << DMI_DMCONTROL_HARTSEL_OFFSET)
#define DMI_DMCONTROL_AUTHENTICATED_OFFSET  7
#define DMI_DMCONTROL_AUTHENTICATED_LENGTH  1
#define DMI_DMCONTROL_AUTHENTICATED         (0x1 << DMI_DMCONTROL_AUTHENTICATED_OFFSET)
#define DMI_DMCONTROL_AUTHBUSY_OFFSET       6
#define DMI_DMCONTROL_AUTHBUSY_LENGTH       1
#define DMI_DMCONTROL_AUTHBUSY              (0x1 << DMI_DMCONTROL_AUTHBUSY_OFFSET)
#define DMI_DMCONTROL_AUTHTYPE_OFFSET       4
#define DMI_DMCONTROL_AUTHTYPE_LENGTH       2
#define DMI_DMCONTROL_AUTHTYPE              (0x3 << DMI_DMCONTROL_AUTHTYPE_OFFSET)
#define DMI_DMCONTROL_VERSION_OFFSET        0
#define DMI_DMCONTROL_VERSION_LENGTH        4
#define DMI_DMCONTROL_VERSION               (0xf << DMI_DMCONTROL_VERSION_OFFSET)
#define DMI_HARTINFO                        0x01
#define DMI_HARTINFO_DATAACCESS_OFFSET      16
#define DMI_HARTINFO_DATAACCESS_LENGTH      1
#define DMI_HARTINFO_DATAACCESS             (0x1 << DMI_HARTINFO_DATAACCESS_OFFSET)
#define DMI_HARTINFO_DATASIZE_OFFSET        12
#define DMI_HARTINFO_DATASIZE_LENGTH        4
#define DMI_HARTINFO_DATASIZE               (0xf << DMI_HARTINFO_DATASIZE_OFFSET)
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
#define DMI_SBCS_SBSINGLEREAD_OFFSET        20
#define DMI_SBCS_SBSINGLEREAD_LENGTH        1
#define DMI_SBCS_SBSINGLEREAD               (0x1 << DMI_SBCS_SBSINGLEREAD_OFFSET)
#define DMI_SBCS_SBACCESS_OFFSET            17
#define DMI_SBCS_SBACCESS_LENGTH            3
#define DMI_SBCS_SBACCESS                   (0x7 << DMI_SBCS_SBACCESS_OFFSET)
#define DMI_SBCS_SBAUTOINCREMENT_OFFSET     16
#define DMI_SBCS_SBAUTOINCREMENT_LENGTH     1
#define DMI_SBCS_SBAUTOINCREMENT            (0x1 << DMI_SBCS_SBAUTOINCREMENT_OFFSET)
#define DMI_SBCS_SBAUTOREAD_OFFSET          15
#define DMI_SBCS_SBAUTOREAD_LENGTH          1
#define DMI_SBCS_SBAUTOREAD                 (0x1 << DMI_SBCS_SBAUTOREAD_OFFSET)
#define DMI_SBCS_SBERROR_OFFSET             12
#define DMI_SBCS_SBERROR_LENGTH             3
#define DMI_SBCS_SBERROR                    (0x7 << DMI_SBCS_SBERROR_OFFSET)
#define DMI_SBCS_SBASIZE_OFFSET             5
#define DMI_SBCS_SBASIZE_LENGTH             7
#define DMI_SBCS_SBASIZE                    (0x7f << DMI_SBCS_SBASIZE_OFFSET)
#define DMI_SBCS_SBACCESS128_OFFSET         4
#define DMI_SBCS_SBACCESS128_LENGTH         1
#define DMI_SBCS_SBACCESS128                (0x1 << DMI_SBCS_SBACCESS128_OFFSET)
#define DMI_SBCS_SBACCESS64_OFFSET          3
#define DMI_SBCS_SBACCESS64_LENGTH          1
#define DMI_SBCS_SBACCESS64                 (0x1 << DMI_SBCS_SBACCESS64_OFFSET)
#define DMI_SBCS_SBACCESS32_OFFSET          2
#define DMI_SBCS_SBACCESS32_LENGTH          1
#define DMI_SBCS_SBACCESS32                 (0x1 << DMI_SBCS_SBACCESS32_OFFSET)
#define DMI_SBCS_SBACCESS16_OFFSET          1
#define DMI_SBCS_SBACCESS16_LENGTH          1
#define DMI_SBCS_SBACCESS16                 (0x1 << DMI_SBCS_SBACCESS16_OFFSET)
#define DMI_SBCS_SBACCESS8_OFFSET           0
#define DMI_SBCS_SBACCESS8_LENGTH           1
#define DMI_SBCS_SBACCESS8                  (0x1 << DMI_SBCS_SBACCESS8_OFFSET)
#define DMI_SBADDRESS0                      0x04
#define DMI_SBADDRESS0_ADDRESS_OFFSET       0
#define DMI_SBADDRESS0_ADDRESS_LENGTH       32
#define DMI_SBADDRESS0_ADDRESS              (0xffffffff << DMI_SBADDRESS0_ADDRESS_OFFSET)
#define DMI_SBADDRESS1                      0x05
#define DMI_SBADDRESS1_ADDRESS_OFFSET       0
#define DMI_SBADDRESS1_ADDRESS_LENGTH       32
#define DMI_SBADDRESS1_ADDRESS              (0xffffffff << DMI_SBADDRESS1_ADDRESS_OFFSET)
#define DMI_SBADDRESS2                      0x06
#define DMI_SBADDRESS2_BUSY_OFFSET          31
#define DMI_SBADDRESS2_BUSY_LENGTH          1
#define DMI_SBADDRESS2_BUSY                 (0x1 << DMI_SBADDRESS2_BUSY_OFFSET)
#define DMI_SBADDRESS2_ADDRESS_OFFSET       0
#define DMI_SBADDRESS2_ADDRESS_LENGTH       31
#define DMI_SBADDRESS2_ADDRESS              (0x7fffffff << DMI_SBADDRESS2_ADDRESS_OFFSET)
#define DMI_SBDATA0                         0x07
#define DMI_SBDATA0_DATA_OFFSET             0
#define DMI_SBDATA0_DATA_LENGTH             32
#define DMI_SBDATA0_DATA                    (0xffffffff << DMI_SBDATA0_DATA_OFFSET)
#define DMI_SBDATA1                         0x08
#define DMI_SBDATA1_DATA_OFFSET             0
#define DMI_SBDATA1_DATA_LENGTH             32
#define DMI_SBDATA1_DATA                    (0xffffffff << DMI_SBDATA1_DATA_OFFSET)
#define DMI_SBDATA2                         0x09
#define DMI_SBDATA2_DATA_OFFSET             0
#define DMI_SBDATA2_DATA_LENGTH             32
#define DMI_SBDATA2_DATA                    (0xffffffff << DMI_SBDATA2_DATA_OFFSET)
#define DMI_SBDATA3                         0x0a
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
#define DMI_ABSTRACTCS_AUTOEXEC0_OFFSET     8
#define DMI_ABSTRACTCS_AUTOEXEC0_LENGTH     1
#define DMI_ABSTRACTCS_AUTOEXEC0            (0x1 << DMI_ABSTRACTCS_AUTOEXEC0_OFFSET)
#define DMI_ABSTRACTCS_CMDERR_OFFSET        5
#define DMI_ABSTRACTCS_CMDERR_LENGTH        3
#define DMI_ABSTRACTCS_CMDERR               (0x7 << DMI_ABSTRACTCS_CMDERR_OFFSET)
#define DMI_ABSTRACTCS_BUSY_OFFSET          4
#define DMI_ABSTRACTCS_BUSY_LENGTH          1
#define DMI_ABSTRACTCS_BUSY                 (0x1 << DMI_ABSTRACTCS_BUSY_OFFSET)
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
#define DMI_SERSTATUS_SERIALCOUNT_OFFSET    28
#define DMI_SERSTATUS_SERIALCOUNT_LENGTH    4
#define DMI_SERSTATUS_SERIALCOUNT           (0xf << DMI_SERSTATUS_SERIALCOUNT_OFFSET)
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
#define DMI_SERSTATUS_VALID0_OFFSET         1
#define DMI_SERSTATUS_VALID0_LENGTH         1
#define DMI_SERSTATUS_VALID0                (0x1 << DMI_SERSTATUS_VALID0_OFFSET)
#define DMI_SERSTATUS_FULL_OVERFLOW0_OFFSET 0
#define DMI_SERSTATUS_FULL_OVERFLOW0_LENGTH 1
#define DMI_SERSTATUS_FULL_OVERFLOW0        (0x1 << DMI_SERSTATUS_FULL_OVERFLOW0_OFFSET)
#define DMI_ACCESSCS                        0x1f
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
#define SERINFO_SERIAL7_OFFSET              7
#define SERINFO_SERIAL7_LENGTH              1
#define SERINFO_SERIAL7                     (0x1 << SERINFO_SERIAL7_OFFSET)
#define SERINFO_SERIAL6_OFFSET              6
#define SERINFO_SERIAL6_LENGTH              1
#define SERINFO_SERIAL6                     (0x1 << SERINFO_SERIAL6_OFFSET)
#define SERINFO_SERIAL5_OFFSET              5
#define SERINFO_SERIAL5_LENGTH              1
#define SERINFO_SERIAL5                     (0x1 << SERINFO_SERIAL5_OFFSET)
#define SERINFO_SERIAL4_OFFSET              4
#define SERINFO_SERIAL4_LENGTH              1
#define SERINFO_SERIAL4                     (0x1 << SERINFO_SERIAL4_OFFSET)
#define SERINFO_SERIAL3_OFFSET              3
#define SERINFO_SERIAL3_LENGTH              1
#define SERINFO_SERIAL3                     (0x1 << SERINFO_SERIAL3_OFFSET)
#define SERINFO_SERIAL2_OFFSET              2
#define SERINFO_SERIAL2_LENGTH              1
#define SERINFO_SERIAL2                     (0x1 << SERINFO_SERIAL2_OFFSET)
#define SERINFO_SERIAL1_OFFSET              1
#define SERINFO_SERIAL1_LENGTH              1
#define SERINFO_SERIAL1                     (0x1 << SERINFO_SERIAL1_OFFSET)
#define SERINFO_SERIAL0_OFFSET              0
#define SERINFO_SERIAL0_LENGTH              1
#define SERINFO_SERIAL0                     (0x1 << SERINFO_SERIAL0_OFFSET)
#define SERSEND0                            0x200
#define SERRECV0                            0x204
#define SERSTAT0                            0x208
#define SERSTAT0_SENDR_OFFSET               1
#define SERSTAT0_SENDR_LENGTH               1
#define SERSTAT0_SENDR                      (0x1 << SERSTAT0_SENDR_OFFSET)
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
#define CSR_TDATA1_TYPE_OFFSET              XLEN-4
#define CSR_TDATA1_TYPE_LENGTH              4
#define CSR_TDATA1_TYPE                     (0xf << CSR_TDATA1_TYPE_OFFSET)
#define CSR_TDATA1_DMODE_OFFSET             XLEN-5
#define CSR_TDATA1_DMODE_LENGTH             1
#define CSR_TDATA1_DMODE                    (0x1 << CSR_TDATA1_DMODE_OFFSET)
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
#define CSR_MCONTROL_TYPE                   (0xf << CSR_MCONTROL_TYPE_OFFSET)
#define CSR_MCONTROL_DMODE_OFFSET           XLEN-5
#define CSR_MCONTROL_DMODE_LENGTH           1
#define CSR_MCONTROL_DMODE                  (0x1 << CSR_MCONTROL_DMODE_OFFSET)
#define CSR_MCONTROL_MASKMAX_OFFSET         XLEN-11
#define CSR_MCONTROL_MASKMAX_LENGTH         6
#define CSR_MCONTROL_MASKMAX                (0x3f << CSR_MCONTROL_MASKMAX_OFFSET)
#define CSR_MCONTROL_SELECT_OFFSET          19
#define CSR_MCONTROL_SELECT_LENGTH          1
#define CSR_MCONTROL_SELECT                 (0x1 << CSR_MCONTROL_SELECT_OFFSET)
#define CSR_MCONTROL_TIMING_OFFSET          18
#define CSR_MCONTROL_TIMING_LENGTH          1
#define CSR_MCONTROL_TIMING                 (0x1 << CSR_MCONTROL_TIMING_OFFSET)
#define CSR_MCONTROL_ACTION_OFFSET          12
#define CSR_MCONTROL_ACTION_LENGTH          6
#define CSR_MCONTROL_ACTION                 (0x3f << CSR_MCONTROL_ACTION_OFFSET)
#define CSR_MCONTROL_CHAIN_OFFSET           11
#define CSR_MCONTROL_CHAIN_LENGTH           1
#define CSR_MCONTROL_CHAIN                  (0x1 << CSR_MCONTROL_CHAIN_OFFSET)
#define CSR_MCONTROL_MATCH_OFFSET           7
#define CSR_MCONTROL_MATCH_LENGTH           4
#define CSR_MCONTROL_MATCH                  (0xf << CSR_MCONTROL_MATCH_OFFSET)
#define CSR_MCONTROL_M_OFFSET               6
#define CSR_MCONTROL_M_LENGTH               1
#define CSR_MCONTROL_M                      (0x1 << CSR_MCONTROL_M_OFFSET)
#define CSR_MCONTROL_H_OFFSET               5
#define CSR_MCONTROL_H_LENGTH               1
#define CSR_MCONTROL_H                      (0x1 << CSR_MCONTROL_H_OFFSET)
#define CSR_MCONTROL_S_OFFSET               4
#define CSR_MCONTROL_S_LENGTH               1
#define CSR_MCONTROL_S                      (0x1 << CSR_MCONTROL_S_OFFSET)
#define CSR_MCONTROL_U_OFFSET               3
#define CSR_MCONTROL_U_LENGTH               1
#define CSR_MCONTROL_U                      (0x1 << CSR_MCONTROL_U_OFFSET)
#define CSR_MCONTROL_EXECUTE_OFFSET         2
#define CSR_MCONTROL_EXECUTE_LENGTH         1
#define CSR_MCONTROL_EXECUTE                (0x1 << CSR_MCONTROL_EXECUTE_OFFSET)
#define CSR_MCONTROL_STORE_OFFSET           1
#define CSR_MCONTROL_STORE_LENGTH           1
#define CSR_MCONTROL_STORE                  (0x1 << CSR_MCONTROL_STORE_OFFSET)
#define CSR_MCONTROL_LOAD_OFFSET            0
#define CSR_MCONTROL_LOAD_LENGTH            1
#define CSR_MCONTROL_LOAD                   (0x1 << CSR_MCONTROL_LOAD_OFFSET)
#define CSR_ICOUNT                          0x7a1
#define CSR_ICOUNT_TYPE_OFFSET              XLEN-4
#define CSR_ICOUNT_TYPE_LENGTH              4
#define CSR_ICOUNT_TYPE                     (0xf << CSR_ICOUNT_TYPE_OFFSET)
#define CSR_ICOUNT_DMODE_OFFSET             XLEN-5
#define CSR_ICOUNT_DMODE_LENGTH             1
#define CSR_ICOUNT_DMODE                    (0x1 << CSR_ICOUNT_DMODE_OFFSET)
#define CSR_ICOUNT_COUNT_OFFSET             10
#define CSR_ICOUNT_COUNT_LENGTH             14
#define CSR_ICOUNT_COUNT                    (0x3fff << CSR_ICOUNT_COUNT_OFFSET)
#define CSR_ICOUNT_M_OFFSET                 9
#define CSR_ICOUNT_M_LENGTH                 1
#define CSR_ICOUNT_M                        (0x1 << CSR_ICOUNT_M_OFFSET)
#define CSR_ICOUNT_H_OFFSET                 8
#define CSR_ICOUNT_H_LENGTH                 1
#define CSR_ICOUNT_H                        (0x1 << CSR_ICOUNT_H_OFFSET)
#define CSR_ICOUNT_S_OFFSET                 7
#define CSR_ICOUNT_S_LENGTH                 1
#define CSR_ICOUNT_S                        (0x1 << CSR_ICOUNT_S_OFFSET)
#define CSR_ICOUNT_U_OFFSET                 6
#define CSR_ICOUNT_U_LENGTH                 1
#define CSR_ICOUNT_U                        (0x1 << CSR_ICOUNT_U_OFFSET)
#define CSR_ICOUNT_ACTION_OFFSET            0
#define CSR_ICOUNT_ACTION_LENGTH            6
#define CSR_ICOUNT_ACTION                   (0x3f << CSR_ICOUNT_ACTION_OFFSET)
#define DTM_IDCODE                          0x01
#define DTM_IDCODE_VERSION_OFFSET           28
#define DTM_IDCODE_VERSION_LENGTH           4
#define DTM_IDCODE_VERSION                  (0xf << DTM_IDCODE_VERSION_OFFSET)
#define DTM_IDCODE_PARTNUMBER_OFFSET        12
#define DTM_IDCODE_PARTNUMBER_LENGTH        16
#define DTM_IDCODE_PARTNUMBER               (0xffff << DTM_IDCODE_PARTNUMBER_OFFSET)
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
#define DTM_DTMCONTROL_DMIRESET_OFFSET      16
#define DTM_DTMCONTROL_DMIRESET_LENGTH      1
#define DTM_DTMCONTROL_DMIRESET             (0x1 << DTM_DTMCONTROL_DMIRESET_OFFSET)
#define DTM_DTMCONTROL_IDLE_OFFSET          12
#define DTM_DTMCONTROL_IDLE_LENGTH          3
#define DTM_DTMCONTROL_IDLE                 (0x7 << DTM_DTMCONTROL_IDLE_OFFSET)
#define DTM_DTMCONTROL_DMISTAT_OFFSET       10
#define DTM_DTMCONTROL_DMISTAT_LENGTH       2
#define DTM_DTMCONTROL_DMISTAT              (0x3 << DTM_DTMCONTROL_DMISTAT_OFFSET)
#define DTM_DTMCONTROL_ABITS_OFFSET         4
#define DTM_DTMCONTROL_ABITS_LENGTH         6
#define DTM_DTMCONTROL_ABITS                (0x3f << DTM_DTMCONTROL_ABITS_OFFSET)
#define DTM_DTMCONTROL_VERSION_OFFSET       0
#define DTM_DTMCONTROL_VERSION_LENGTH       4
#define DTM_DTMCONTROL_VERSION              (0xf << DTM_DTMCONTROL_VERSION_OFFSET)
#define DTM_DMI                             0x11
#define DTM_DMI_ADDRESS_OFFSET              34
#define DTM_DMI_ADDRESS_LENGTH              abits
#define DTM_DMI_ADDRESS                     (((1L<<abits)-1) << DTM_DMI_ADDRESS_OFFSET)
#define DTM_DMI_DATA_OFFSET                 2
#define DTM_DMI_DATA_LENGTH                 32
#define DTM_DMI_DATA                        (0xffffffff << DTM_DMI_DATA_OFFSET)
#define DTM_DMI_OP_OFFSET                   0
#define DTM_DMI_OP_LENGTH                   2
#define DTM_DMI_OP                          (0x3 << DTM_DMI_OP_OFFSET)
#define SHORTNAME                           0x123
#define SHORTNAME_FIELD_OFFSET              0
#define SHORTNAME_FIELD_LENGTH              8
#define SHORTNAME_FIELD                     (0xff << SHORTNAME_FIELD_OFFSET)
#define TRACE                               0x728
#define TRACE_WRAPPED_OFFSET                24
#define TRACE_WRAPPED_LENGTH                1
#define TRACE_WRAPPED                       (0x1 << TRACE_WRAPPED_OFFSET)
#define TRACE_EMITTIMESTAMP_OFFSET          23
#define TRACE_EMITTIMESTAMP_LENGTH          1
#define TRACE_EMITTIMESTAMP                 (0x1 << TRACE_EMITTIMESTAMP_OFFSET)
#define TRACE_EMITSTOREDATA_OFFSET          22
#define TRACE_EMITSTOREDATA_LENGTH          1
#define TRACE_EMITSTOREDATA                 (0x1 << TRACE_EMITSTOREDATA_OFFSET)
#define TRACE_EMITLOADDATA_OFFSET           21
#define TRACE_EMITLOADDATA_LENGTH           1
#define TRACE_EMITLOADDATA                  (0x1 << TRACE_EMITLOADDATA_OFFSET)
#define TRACE_EMITSTOREADDR_OFFSET          20
#define TRACE_EMITSTOREADDR_LENGTH          1
#define TRACE_EMITSTOREADDR                 (0x1 << TRACE_EMITSTOREADDR_OFFSET)
#define TRACE_EMITLOADADDR_OFFSET           19
#define TRACE_EMITLOADADDR_LENGTH           1
#define TRACE_EMITLOADADDR                  (0x1 << TRACE_EMITLOADADDR_OFFSET)
#define TRACE_EMITPRIV_OFFSET               18
#define TRACE_EMITPRIV_LENGTH               1
#define TRACE_EMITPRIV                      (0x1 << TRACE_EMITPRIV_OFFSET)
#define TRACE_EMITBRANCH_OFFSET             17
#define TRACE_EMITBRANCH_LENGTH             1
#define TRACE_EMITBRANCH                    (0x1 << TRACE_EMITBRANCH_OFFSET)
#define TRACE_EMITPC_OFFSET                 16
#define TRACE_EMITPC_LENGTH                 1
#define TRACE_EMITPC                        (0x1 << TRACE_EMITPC_OFFSET)
#define TRACE_FULLACTION_OFFSET             8
#define TRACE_FULLACTION_LENGTH             2
#define TRACE_FULLACTION                    (0x3 << TRACE_FULLACTION_OFFSET)
#define TRACE_DESTINATION_OFFSET            4
#define TRACE_DESTINATION_LENGTH            2
#define TRACE_DESTINATION                   (0x3 << TRACE_DESTINATION_OFFSET)
#define TRACE_STALL_OFFSET                  2
#define TRACE_STALL_LENGTH                  1
#define TRACE_STALL                         (0x1 << TRACE_STALL_OFFSET)
#define TRACE_DISCARD_OFFSET                1
#define TRACE_DISCARD_LENGTH                1
#define TRACE_DISCARD                       (0x1 << TRACE_DISCARD_OFFSET)
#define TRACE_SUPPORTED_OFFSET              0
#define TRACE_SUPPORTED_LENGTH              1
#define TRACE_SUPPORTED                     (0x1 << TRACE_SUPPORTED_OFFSET)
#define TBUFSTART                           0x729
#define TBUFEND                             0x72a
#define TBUFWRITE                           0x72b
