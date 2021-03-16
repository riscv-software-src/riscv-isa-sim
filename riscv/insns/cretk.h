require_privilege(PRV_S);
reg_t w0 = p->get_csr(CSR_SCRTKEYH);  // h
reg_t k0 = p->get_csr(CSR_SCRTKEYL);  // l
reg_t result = qarma64_enc(RS1, RS2, w0, k0, 7);
WRITE_RD(result);
