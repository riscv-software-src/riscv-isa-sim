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
#include "test_pmp_ok_share_1.h"
#include "test_pmp_csr_1.h"

#define GEN_ALL 1

namespace {

const unsigned expected_files_count[] = {
        128,
        528,
        24,
        0
};

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
                                for (int val = 0; val < 8; val++) {
                                    if (val == 0 && test_pmp) continue;    // skip, since no change
#if TEST_RW01_ONLY
    if (test_pmp) {
        if ((idx == 0 && (val & 0x3) == 0x1) || (idx == 1 && (val & 0x3) == 0x2)) {
            // test RW=01;
        } else {
            continue;
        }
    }
#endif

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
        gen_class_2.set_revert_rwx(val);

        if (idx == 0) { // for cfg2 and cfg3, since PMP_L might set there
            int sub_idx = 2 + cur_files_count % 2;
            gen_class_2.set_addr_idx(sub_idx);
            gen_class_2.set_addr_offset(0);
            gen_class_2.set_cfg_idx(0);
            gen_class_2.set_cfg_sub_idx(sub_idx);

            if (pmp_lock && !pre_rlb) {
                pmpcfg_fail = 1;
                pmpaddr_fail = 1;
            }

            if (!pre_mml && (val & 0x3) == 0x1) { // b'11^01 = 10, RW=01
                pmpcfg_fail = 1;
            }
        } else {    // for invalid cfgs
            gen_class_2.set_addr_idx(4 + cur_files_count % (max_pmp - 4));
            gen_class_2.set_addr_offset(0x10000);
            gen_class_2.set_cfg_idx((1 + cur_files_count % (max_pmp_cfg - 1)) * 2);   // for 2, 4, ..., 14
            gen_class_2.set_cfg_sub_idx((cur_files_count >> val) % 4);
            if (!pre_mml && (val & 0x3) == 0x2) { // b'00^10 = 10, RW=01
                pmpcfg_fail = 1;
            }
        }

        if (pmpcfg_fail || pmpaddr_fail) cur_expected_errors += 1;
    } else {            // seccfg test
        unsigned sec_val = val;
        unsigned sec_rlb = (sec_val >> 2) & 0x1;
        unsigned sec_mml = (sec_val >> 0) & 0x1;
        unsigned sec_mmwp = (sec_val >> 1) & 0x1;
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

#if GEN_ALL
    pmp_ok_share_1_gen_class gen_class_3;
    for (int r = 0; r < 2; r++) {
        for (int x = 0; x < 2; x++) {
            for (int cfgl = 0; cfgl < 2; cfgl++) {
                for (int typex = 0; typex < 2; typex++) {
                    for (int umode = 0; umode < 2; umode++) {
    // not share mode and M mode
    if (r == 1 && umode == 0) continue;

    str_buffer.str("");
    str_buffer << "outputs/test_pmp_ok_share_1_r" << r << "_x" << x << "_cfgl" << cfgl
            << "_typex" << typex << "_umode" << umode << ".c";
    m_ofstream.open(str_buffer.str().c_str());
    cur_files_count++;

    gen_class_3.set_tag(str_buffer.str());

    unsigned r_err = 0;
    unsigned w_err = 0;
    unsigned x_err = 0;

    gen_class_3.set_pmp_r(r);
    gen_class_3.set_pmp_x(x);
    gen_class_3.set_pmp_l(cfgl);
    gen_class_3.set_typex(typex);
    gen_class_3.set_enable_umode_test(umode);

    if (r != 0) {   // not share mode
        if (typex == 0) {
            r_err = 1;
            w_err = 1;
        } else {
            x_err = 1;
        }
    } else {
        if (cfgl) {
            if (typex == 0) {
                if (x == 0) {
                    // no RW access
                    r_err = 1;
                    w_err = 1;
                } else {
                    // readable for M mode
                    if (umode) {
                        r_err = 1;
                        w_err = 1;
                    } else {
                        w_err = 1;
                    }
                }
            } else {
                // always executable
            }
        } else {
            if (typex == 0) {
                if (x == 0) {
                    // RW M mode, R for U
                    if (umode) {
                        w_err = 1;
                    }
                }
            } else {
                x_err = 1;  // when !cfgl, not executable
            }
        }
    }

    cur_expected_errors += r_err + w_err + x_err;
    gen_class_3.set_expected_r_fail(r_err);
    gen_class_3.set_expected_w_fail(w_err);
    gen_class_3.set_expected_x_fail(x_err);

    str_buffer.str("");
    gen_class_3.generate_pmp_ok_share_1(str_buffer, 0);
    str_buffer << std::endl;
    m_ofstream << str_buffer.str();
    m_ofstream.close();
                    }
                }
            }
        }
    }
#endif

#if GEN_ALL
    unsigned expectedCount = 0;
    for (int i=0; i<sizeof(expected_files_count)/sizeof(expected_files_count[0]); i++) {
        expectedCount += expected_files_count[i];
    }
    if (expectedCount != cur_files_count) {
        std::cerr << std::dec << "Total " << cur_files_count << " files generated, v.s. expected "
                << expectedCount << std::endl;
        exit(1);
    } else {
        std::cout << std::dec << "Total " << expectedCount << " files generated as expected." << std::endl;
    }
#endif
    printf("Generates expected errors %d.\n", cur_expected_errors);
    return 0;
}

