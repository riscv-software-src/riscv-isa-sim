/*
 * insn_utest-1.cc
 *
 *  Created on: Mar.6 2020
 *      Author: soberl
 */
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <stdlib.h>
#include <assert.h>

#include "test_pmp_ok_1.h"
#include "test_pmp_csr_1.h"

#define GEN_ALL 1

namespace {
    std::ostringstream str_buffer, val_buffer;
    std::ofstream m_ofstream;
    unsigned cur_files_count = 0;
    unsigned cur_expected_errors = 0;

    const int max_pmp = 16;  // from spike
    const int max_pmp_cfg = max_pmp / 8;  // for RV64
};



int
main()
{
#if GEN_ALL
    pmp_ok_1_gen_class gen_class_1;

    for (int u_mode = 0; u_mode < 2; u_mode++) {
        for (int rw = 0; rw < 2; rw++) {
            for (int x = 0; x < 2; x++) {
                for (int cfgl = 0; cfgl < 2; cfgl++) {
                    for (int pmp_match = 0; pmp_match < 2; pmp_match++) {
                        for (int mmwp = 0; mmwp < 2; mmwp++) {
                            for (int mml = 0; mml < 2; mml++) {
    str_buffer.str("");
    str_buffer << "outputs/test_pmp_ok_1_u" << u_mode << "_rw" << rw << "_x" << x << "_l" << cfgl
            << "_match" << pmp_match << "_mmwp" << mmwp << "_mml" << mml << ".c";
    m_ofstream.open(str_buffer.str().c_str());
    cur_files_count++;

    gen_class_1.set_tag(str_buffer.str());

    unsigned rw_err = 0;
    unsigned x_err = 0;

    gen_class_1.set_switch_u_mode(u_mode);
    gen_class_1.set_pmp_rw(rw);
    gen_class_1.set_pmp_x(x);
    gen_class_1.set_pmp_l(cfgl);

    gen_class_1.set_set_sec_mmwp(mmwp);
    gen_class_1.set_set_sec_mml(mml);

    if (pmp_match) {
        gen_class_1.set_create_pmp_cfg(pmp_match);
        gen_class_1.set_pmp_addr_offset(0);
        if (mml) {
            if (1 - u_mode != cfgl) {
                rw_err = 1;
                x_err = 1;
            }
            if (rw == 0) rw_err = 1;
            if (x == 0) x_err = 1;
        } else {
            if (u_mode == 1 || cfgl) {
                if (rw == 0) rw_err = 1;
                if (x == 0) x_err = 1;
            }
        }
    } else {
        if (cur_files_count % 3 == 0) {
            gen_class_1.set_create_pmp_cfg(1);
            gen_class_1.set_pmp_addr_offset(0x100); // >= sizeof(.test) section
        } else {
            gen_class_1.set_create_pmp_cfg(0);
        }
        if (u_mode == 1 || mmwp) { // mmwp to against non-match
            rw_err = 1;
            x_err = 1;
        }
    }

    cur_expected_errors += rw_err + x_err;
    gen_class_1.set_expected_rw_fail(rw_err);
    gen_class_1.set_expected_x_fail(x_err);

    str_buffer.str("");
    gen_class_1.generate_pmp_ok_1(str_buffer, 0);
    str_buffer << std::endl;
    m_ofstream << str_buffer.str();
    m_ofstream.close();
                            }
                        }
                    }
                }
            }
        }
    }
#endif

#if GEN_ALL
    pmp_csr_1_gen_class gen_class_2;

    for (int pmp_lock = 0; pmp_lock < 2; pmp_lock++) {
        for (int lock_once = 0; lock_once < 2; lock_once++) {
            if (pmp_lock == 1 && lock_once == 1) continue;  // locked once anyway
            for (int pre_rlb = 0; pre_rlb < 2; pre_rlb++) {
                for (int pre_mmwp = 0; pre_mmwp < 2; pre_mmwp++) {
                    for (int pre_mml = 0; pre_mml < 2; pre_mml++) {
                        for (int test_pmp = 0; test_pmp < 2; test_pmp++) {
                            for (int idx = 0; idx < 2; idx++) {
                                if (test_pmp == 0 && idx == 1) continue;   // only 1 seccfg
                                for (int val = 0; val < (test_pmp ? 2 : 8); val++) {

    str_buffer.str("");
    str_buffer << "outputs/test_pmp_csr_1_lock" << pmp_lock << lock_once
            << "_rlb" << pre_rlb << "_mmwp" << pre_mmwp << "_mml" << pre_mml
            << (test_pmp ? "_pmp_" : "_sec_") << idx << val
            << ".c";
    m_ofstream.open(str_buffer.str().c_str());
    cur_files_count++;

    gen_class_2.set_tag(str_buffer.str());

    gen_class_2.set_pmp_lock(pmp_lock);
    gen_class_2.set_lock_once(lock_once);

    gen_class_2.set_lock_bypass(pre_rlb);
    gen_class_2.set_pre_sec_mml(pre_mml);
    gen_class_2.set_pre_sec_mmwp(pre_mmwp);

    gen_class_2.set_group_pmp(test_pmp);

    int pmpcfg_fail = 0;
    int pmpaddr_fail = 0;
    int seccfg_fail = 0;

    if (test_pmp == 1) {   // pmpcfg and pmpaddr test
        if (idx == 0) { // for cfg2 and cfg3, since PMP_L might set there
            int sub_idx = 2 + cur_files_count % 2;
            gen_class_2.set_addr_idx(sub_idx);
            gen_class_2.set_addr_offset(0);
            gen_class_2.set_cfg_idx(0);
            gen_class_2.set_cfg_sub_idx(sub_idx);

            if (pmp_lock && !pre_rlb) {
                pmpcfg_fail = 1;
                pmpaddr_fail = 1;
                cur_expected_errors += 1;
            }
        } else {    // for invalid cfgs
            gen_class_2.set_addr_idx(4 + cur_files_count % (max_pmp - 4));
            gen_class_2.set_addr_offset(0x10000);
            gen_class_2.set_cfg_idx((1 + cur_files_count % (max_pmp_cfg - 1)) * 2);   // for 2, 4, ..., 14
            gen_class_2.set_cfg_sub_idx((cur_files_count >> val) % 4);
        }
    } else {            // seccfg test
        unsigned sec_val = val;
        unsigned sec_rlb = sec_val & 0x1;
        unsigned sec_mml = (sec_val >> 1) & 0x1;
        unsigned sec_mmwp = (sec_val >> 2) & 0x1;
        gen_class_2.set_sec_rlb(sec_rlb);
        gen_class_2.set_sec_mml(sec_mml);
        gen_class_2.set_sec_mmwp(sec_mmwp);
    }

    gen_class_2.set_expected_seccfg_fail(seccfg_fail);
    gen_class_2.set_expected_pmpaddr_fail(pmpaddr_fail);
    gen_class_2.set_expected_pmpcfg_fail(pmpcfg_fail);

    str_buffer.str("");
    gen_class_2.generate_pmp_csr_1(str_buffer, 0);
    str_buffer << std::endl;
    m_ofstream << str_buffer.str();
    m_ofstream.close();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#endif

    printf("Total %d files generated, expected errors %d.\n", cur_files_count, cur_expected_errors);
    return 0;
}

