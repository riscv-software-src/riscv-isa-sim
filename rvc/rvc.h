#ifndef _RVC_H
#define _RVC_H

#define INSN_IS_RVC(x) (((x) & 0x3) < 0x3)
#define insn_length(x) (INSN_IS_RVC(x) ? 2 : 4)
#define require_rvc if(!(sr & SR_EC)) throw_illegal_instruction

#define CRD_REGNUM ((insn.bits >> 5) & 0x1f)
#define CRD XPR.write_port(CRD_REGNUM)
#define CRS1 XPR[(insn.bits >> 10) & 0x1f]
#define CRS2 XPR[(insn.bits >> 5) & 0x1f]
#define CIMM6 ((int32_t)((insn.bits >> 10) & 0x3f) << 26 >> 26)
#define CIMM5U ((insn.bits >> 5) & 0x1f)
#define CIMM5 ((int32_t)CIMM5U << 27 >> 27)
#define CIMM10 ((int32_t)((insn.bits >> 5) & 0x3ff) << 22 >> 22)
#define CBRANCH_TARGET (pc + (CIMM5 << BRANCH_ALIGN_BITS))
#define CJUMP_TARGET (pc + (CIMM10 << JUMP_ALIGN_BITS))

static const int rvc_rs1_regmap[8] = { 20, 21, 2, 3, 4, 5, 6, 7 };
#define rvc_rd_regmap rvc_rs1_regmap
#define rvc_rs2b_regmap rvc_rs1_regmap
static const int rvc_rs2_regmap[8] = { 20, 21, 2, 3, 4, 5, 6, 0 };
#define CRDS XPR.write_port(rvc_rd_regmap[(insn.bits >> 13) & 0x7])
#define FCRDS FPR.write_port(rvc_rd_regmap[(insn.bits >> 13) & 0x7])
#define CRS1S XPR[rvc_rs1_regmap[(insn.bits >> 10) & 0x7]]
#define CRS2S XPR[rvc_rs2_regmap[(insn.bits >> 13) & 0x7]]
#define CRS2BS XPR[rvc_rs2b_regmap[(insn.bits >> 5) & 0x7]]
#define FCRS2S FPR[rvc_rs2_regmap[(insn.bits >> 13) & 0x7]]

#endif
