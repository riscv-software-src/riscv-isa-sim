#include "isa_parser.h"
#include <stdexcept>

static std::string strtolower(const char* str)
{
  std::string res;
  for (const char *r = str; *r; r++)
    res += std::tolower(*r);
  return res;
}

static unsigned long safe_stoul(const std::string& s)
{
  int old_errno = errno;
  errno = 0;

  char* endp;
  unsigned long ret = strtoul(s.c_str(), &endp, 10);

  int new_errno = errno;
  errno = old_errno;

  if (endp == s.c_str() || *endp)
    throw std::invalid_argument("stoul");

  if (new_errno)
    throw std::out_of_range("stoul");

  return ret;
}

static void bad_option_string(const char *option, const char *value,
                              const char *msg)
{
  fprintf(stderr, "error: bad %s option '%s'. %s\n", option, value, msg);
  abort();
}

static void bad_isa_string(const char* isa, const char* msg)
{
  bad_option_string("--isa", isa, msg);
}

static void bad_priv_string(const char* priv)
{
  fprintf(stderr, "error: bad --priv option %s\n", priv);
  abort();
}

isa_parser_t::isa_parser_t(const char* str, const char *priv)
{
  isa_string = strtolower(str);
  const char* all_subsets = "mafdqcbpvh";

  if (isa_string.compare(0, 4, "rv32") == 0)
    max_xlen = 32;
  else if (isa_string.compare(0, 4, "rv64") == 0)
    max_xlen = 64;
  else
    bad_isa_string(str, "ISA strings must begin with RV32 or RV64");

  vlen = 0;
  elen = 0;
  zvf = false;
  zvd = false;

  switch (isa_string[4]) {
    case 'g':
      // G = IMAFD_Zicsr_Zifencei, but Spike includes the latter two
      // unconditionally, so they need not be explicitly added here.
      isa_string = isa_string.substr(0, 4) + "imafd" + isa_string.substr(5);
      // Fall through
    case 'i':
      extension_table['I'] = true;
      break;

    case 'e':
      extension_table['E'] = true;
      break;

    default:
      bad_isa_string(str, ("'" + isa_string.substr(0, 4) + "' must be followed by I, E, or G").c_str());
  }

  const char* isa_str = isa_string.c_str();
  auto p = isa_str, subset = all_subsets;
  for (p += 5; islower(*p) && !strchr("zsx", *p); ++p) {
    while (*subset && (*p != *subset))
      ++subset;

    if (!*subset) {
      if (strchr(all_subsets, *p))
        bad_isa_string(str, ("Extension '" + std::string(1, *p) + "' appears too late in ISA string").c_str());
      else
        bad_isa_string(str, ("Unsupported extension '" + std::string(1, *p) + "'").c_str());
    }

    switch (*p) {
      case 'v': vlen = 128; elen = 64; zvf = true; zvd = true;
                // even rv32iv implies double float
      case 'q': extension_table['D'] = true;
                // Fall through
      case 'd': extension_table['F'] = true;
    }
    extension_table[toupper(*p)] = true;
    while (isdigit(*(p + 1))) {
      ++p; // skip major version, point, and minor version if presented
      if (*(p + 1) == 'p') ++p;
    }
    p += *(p + 1) == '_'; // underscores may be used to improve readability
  }

  while (islower(*p) || (*p == '_')) {
    p += *p == '_'; // first underscore is optional
    auto end = p;
    do ++end; while (*end && *end != '_');
    auto ext_str = std::string(p, end);
    if (ext_str == "zfh" || ext_str == "zfhmin") {
      if (!extension_table['F'])
        bad_isa_string(str, ("'" + ext_str + "' extension requires 'F'").c_str());
      extension_table[EXT_ZFHMIN] = true;
      if (ext_str == "zfh")
        extension_table[EXT_ZFH] = true;
    } else if (ext_str == "zvfh" || ext_str == "zvfhmin") {
      extension_table[EXT_ZVFHMIN] = true;

      if (ext_str == "zvfh") {
        extension_table[EXT_ZVFH] = true;
        // Zvfh implies Zfhmin
        extension_table[EXT_ZFHMIN] = true;
      }
    } else if (ext_str == "zicsr") {
      // Spike necessarily has Zicsr, because
      // Zicsr is implied by the privileged architecture
    } else if (ext_str == "zifencei") {
      // For compatibility with version 2.0 of the base ISAs, we
      // unconditionally include FENCE.I, so Zifencei adds nothing more.
    } else if (ext_str == "zihintpause") {
      // HINTs encoded in base-ISA instructions are always present.
    } else if (ext_str == "zihintntl") {
      // HINTs encoded in base-ISA instructions are always present.
    } else if (ext_str == "zaamo") {
      extension_table[EXT_ZAAMO] = true;
    } else if (ext_str == "zalrsc") {
      extension_table[EXT_ZALRSC] = true;
    } else if (ext_str == "zacas") {
      extension_table[EXT_ZACAS] = true;
    } else if (ext_str == "zabha") {
      extension_table[EXT_ZABHA] = true;
    } else if (ext_str == "zawrs") {
      extension_table[EXT_ZAWRS] = true;
    } else if (ext_str == "zmmul") {
      extension_table[EXT_ZMMUL] = true;
    } else if (ext_str == "zba") {
      extension_table[EXT_ZBA] = true;
    } else if (ext_str == "zbb") {
      extension_table[EXT_ZBB] = true;
    } else if (ext_str == "zbc") {
      extension_table[EXT_ZBC] = true;
    } else if (ext_str == "zbs") {
      extension_table[EXT_ZBS] = true;
    } else if (ext_str == "zbkb") {
      extension_table[EXT_ZBKB] = true;
    } else if (ext_str == "zbkc") {
      extension_table[EXT_ZBKC] = true;
    } else if (ext_str == "zbkx") {
      extension_table[EXT_ZBKX] = true;
    } else if (ext_str == "zdinx") {
      extension_table[EXT_ZFINX] = true;
      extension_table[EXT_ZDINX] = true;
    } else if (ext_str == "zfbfmin") {
      extension_table[EXT_ZFBFMIN] = true;
    } else if (ext_str == "zfinx") {
      extension_table[EXT_ZFINX] = true;
    } else if (ext_str == "zhinx") {
      extension_table[EXT_ZFINX] = true;
      extension_table[EXT_ZHINX] = true;
      extension_table[EXT_ZHINXMIN] = true;
    } else if (ext_str == "zhinxmin") {
      extension_table[EXT_ZFINX] = true;
      extension_table[EXT_ZHINXMIN] = true;
    } else if (ext_str == "zce") {
      extension_table[EXT_ZCA] = true;
      extension_table[EXT_ZCB] = true;
      extension_table[EXT_ZCMT] = true;
      extension_table[EXT_ZCMP] = true;
      if (extension_table['F'] && max_xlen == 32)
        extension_table[EXT_ZCF] = true;
    } else if (ext_str == "zca") {
      extension_table[EXT_ZCA] = true;
    } else if (ext_str == "zcf") {
      if (max_xlen != 32)
        bad_isa_string(str, "'Zcf' requires RV32");
      extension_table[EXT_ZCF] = true;
    } else if (ext_str == "zcb") {
      extension_table[EXT_ZCB] = true;
    } else if (ext_str == "zcd") {
      extension_table[EXT_ZCD] = true;
    } else if (ext_str == "zcmp") {
      extension_table[EXT_ZCMP] = true;
    } else if (ext_str == "zcmt") {
      extension_table[EXT_ZCMT] = true;
    } else if (ext_str == "zk") {
      extension_table[EXT_ZBKB] = true;
      extension_table[EXT_ZBKC] = true;
      extension_table[EXT_ZBKX] = true;
      extension_table[EXT_ZKND] = true;
      extension_table[EXT_ZKNE] = true;
      extension_table[EXT_ZKNH] = true;
      extension_table[EXT_ZKR] = true;
    } else if (ext_str == "zkn") {
      extension_table[EXT_ZBKB] = true;
      extension_table[EXT_ZBKC] = true;
      extension_table[EXT_ZBKX] = true;
      extension_table[EXT_ZKND] = true;
      extension_table[EXT_ZKNE] = true;
      extension_table[EXT_ZKNH] = true;
    } else if (ext_str == "zknd") {
      extension_table[EXT_ZKND] = true;
    } else if (ext_str == "zkne") {
      extension_table[EXT_ZKNE] = true;
    } else if (ext_str == "zknh") {
      extension_table[EXT_ZKNH] = true;
    } else if (ext_str == "zks") {
      extension_table[EXT_ZBKB] = true;
      extension_table[EXT_ZBKC] = true;
      extension_table[EXT_ZBKX] = true;
      extension_table[EXT_ZKSED] = true;
      extension_table[EXT_ZKSH] = true;
    } else if (ext_str == "zksed") {
      extension_table[EXT_ZKSED] = true;
    } else if (ext_str == "zksh") {
      extension_table[EXT_ZKSH] = true;
    } else if (ext_str == "zkr") {
      extension_table[EXT_ZKR] = true;
    } else if (ext_str == "zkt") {
    } else if (ext_str == "smepmp") {
      extension_table[EXT_SMEPMP] = true;
    } else if (ext_str == "smstateen") {
      extension_table[EXT_SMSTATEEN] = true;
    } else if (ext_str == "smrnmi") {
      extension_table[EXT_SMRNMI] = true;
    } else if (ext_str == "sscofpmf") {
      extension_table[EXT_SSCOFPMF] = true;
    } else if (ext_str == "svadu") {
      extension_table[EXT_SVADU] = true;
    } else if (ext_str == "svnapot") {
      extension_table[EXT_SVNAPOT] = true;
    } else if (ext_str == "svpbmt") {
      extension_table[EXT_SVPBMT] = true;
    } else if (ext_str == "svinval") {
      extension_table[EXT_SVINVAL] = true;
    } else if (ext_str == "zfa") {
      extension_table[EXT_ZFA] = true;
    } else if (ext_str == "zicbom") {
      extension_table[EXT_ZICBOM] = true;
    } else if (ext_str == "zicboz") {
      extension_table[EXT_ZICBOZ] = true;
    } else if (ext_str == "zicbop") {
    } else if (ext_str == "zicntr") {
      extension_table[EXT_ZICNTR] = true;
    } else if (ext_str == "zicond") {
      extension_table[EXT_ZICOND] = true;
    } else if (ext_str == "zihpm") {
      extension_table[EXT_ZIHPM] = true;
    } else if (ext_str == "zilsd") {
      if (max_xlen != 32)
        bad_isa_string(str, "'Zilsd' requires RV32");
      extension_table[EXT_ZILSD] = true;
    } else if (ext_str == "zcmlsd") {
      extension_table[EXT_ZCMLSD] = true;
    } else if (ext_str == "zvbb") {
      extension_table[EXT_ZVBB] = true;
    } else if (ext_str == "zvbc") {
      extension_table[EXT_ZVBC] = true;
    } else if (ext_str == "zvfbfmin") {
      extension_table[EXT_ZVFBFMIN] = true;
    } else if (ext_str == "zvfbfwma") {
      extension_table[EXT_ZVFBFWMA] = true;
    } else if (ext_str == "zvkg") {
      extension_table[EXT_ZVKG] = true;
    } else if (ext_str == "zvkn") {
      extension_table[EXT_ZVBB] = true;
      extension_table[EXT_ZVKNED] = true;
      extension_table[EXT_ZVKNHB] = true;
    } else if (ext_str == "zvknc") {
      extension_table[EXT_ZVBB] = true;
      extension_table[EXT_ZVBC] = true;
      extension_table[EXT_ZVKNED] = true;
      extension_table[EXT_ZVKNHB] = true;
    } else if (ext_str == "zvkng") {
      extension_table[EXT_ZVBB] = true;
      extension_table[EXT_ZVKG] = true;
      extension_table[EXT_ZVKNED] = true;
      extension_table[EXT_ZVKNHB] = true;
    } else if (ext_str == "zvkned") {
      extension_table[EXT_ZVKNED] = true;
    } else if (ext_str == "zvknha") {
      extension_table[EXT_ZVKNHA] = true;
    } else if (ext_str == "zvknhb") {
      extension_table[EXT_ZVKNHB] = true;
    } else if (ext_str == "zvks") {
      extension_table[EXT_ZVBB] = true;
      extension_table[EXT_ZVKSED] = true;
      extension_table[EXT_ZVKSH] = true;
    } else if (ext_str == "zvksc") {
      extension_table[EXT_ZVBB] = true;
      extension_table[EXT_ZVBC] = true;
      extension_table[EXT_ZVKSED] = true;
      extension_table[EXT_ZVKSH] = true;
    } else if (ext_str == "zvksg") {
      extension_table[EXT_ZVBB] = true;
      extension_table[EXT_ZVKG] = true;
      extension_table[EXT_ZVKSED] = true;
      extension_table[EXT_ZVKSH] = true;
    } else if (ext_str == "zvksed") {
      extension_table[EXT_ZVKSED] = true;
    } else if (ext_str == "zvksh") {
      extension_table[EXT_ZVKSH] = true;
    } else if (ext_str == "zvkt") {
    } else if (ext_str == "sstc") {
        extension_table[EXT_SSTC] = true;
    } else if (ext_str == "smcsrind") {
      extension_table[EXT_SMCSRIND] = true;
    } else if (ext_str == "sscsrind") {
      extension_table[EXT_SSCSRIND] = true;
    } else if (ext_str == "smcntrpmf") {
      extension_table[EXT_SMCNTRPMF] = true;
    } else if (ext_str == "zimop") {
      extension_table[EXT_ZIMOP] = true;
    } else if (ext_str == "zcmop") {
      extension_table[EXT_ZCMOP] = true;
    } else if (ext_str == "zalasr") {
      extension_table[EXT_ZALASR] = true;
    } else if (ext_str == "ssqosid") {
      extension_table[EXT_SSQOSID] = true;
    } else if (ext_str == "zicfilp") {
      extension_table[EXT_ZICFILP] = true;
    } else if (ext_str == "zicfiss") {
      extension_table[EXT_ZICFISS] = true;
    } else if (ext_str == "smmpm") {
      extension_table[EXT_SMMPM] = true;
    } else if (ext_str == "smnpm") {
      extension_table[EXT_SMNPM] = true;
    } else if (ext_str == "ssnpm") {
      extension_table[EXT_SSNPM] = true;
    } else if (ext_str.substr(0, 3) == "zvl") {
      reg_t new_vlen;
      try {
        new_vlen = safe_stoul(ext_str.substr(3, ext_str.size() - 4));
      } catch (std::logic_error& e) {
        new_vlen = 0;
      }
      if ((new_vlen & (new_vlen - 1)) != 0 || new_vlen < 32 || ext_str.back() != 'b')
        bad_isa_string(str, ("Invalid Zvl string: " + ext_str).c_str());
      vlen = std::max(vlen, new_vlen);
    } else if (ext_str.substr(0, 3) == "zve") {
      reg_t new_elen;
      try {
        new_elen = safe_stoul(ext_str.substr(3, ext_str.size() - 4));
      } catch (std::logic_error& e) {
        new_elen = 0;
      }
      if (ext_str.substr(5) == "d") {
        zvd |= true; zvf |= true;
      } else if (ext_str.substr(5) == "f") {
        zvf |= true;
      } else if (ext_str.substr(5) == "x") {
        /* do nothing */
      } else {
        new_elen = 0;
      }
      if (new_elen != 32 && new_elen != 64)
        bad_isa_string(str, ("Invalid Zve string: " + ext_str).c_str());
      elen = std::max(elen, new_elen);
    } else if (ext_str == "ssdbltrp") {
      extension_table[EXT_SSDBLTRP] = true;
    } else if (ext_str == "smdbltrp") {
      extension_table[EXT_SMDBLTRP] = true;
    } else if (ext_str[0] == 'x') {
      extension_table['X'] = true;
      if (ext_str.size() == 1) {
        bad_isa_string(str, "single 'X' is not a proper name");
      } else if (ext_str != "xdummy") {
        extensions.insert(ext_str.substr(1));
      }
    } else {
      bad_isa_string(str, ("unsupported extension: " + ext_str).c_str());
    }
    p = end;
  }
  if (*p) {
    bad_isa_string(str, ("can't parse: " + std::string(p)).c_str());
  }

  if (extension_table['A']) {
    extension_table[EXT_ZAAMO] = true;
    extension_table[EXT_ZALRSC] = true;
  } else if (extension_table[EXT_ZAAMO] && extension_table[EXT_ZALRSC]) {
    extension_table['A'] = true;
  }

  if (extension_table['B']) {
    extension_table[EXT_ZBA] = true;
    extension_table[EXT_ZBB] = true;
    extension_table[EXT_ZBS] = true;
  } else if (extension_table[EXT_ZBA] && extension_table[EXT_ZBB] && extension_table[EXT_ZBS]) {
    extension_table['B'] = true;
  }

  if (extension_table['C']) {
    extension_table[EXT_ZCA] = true;
    if (extension_table['F'] && max_xlen == 32)
      extension_table[EXT_ZCF] = true;
    if (extension_table['D'])
      extension_table[EXT_ZCD] = true;
  }

  if (extension_table[EXT_ZCMLSD] && extension_table[EXT_ZCF]) {
    bad_isa_string(str, "'Zcmlsd' extension conflicts with 'Zcf' extensions");
  }

  if (extension_table[EXT_ZCMLSD] && (!extension_table[EXT_ZCA] || !extension_table[EXT_ZILSD])) {
    bad_isa_string(str, "'Zcmlsd' extension requires 'Zca' and 'Zilsd' extensions");
  }

  if (extension_table[EXT_ZFBFMIN] && !extension_table['F']) {
    bad_isa_string(str, "'Zfbfmin' extension requires 'F' extension");
  }

  if ((extension_table[EXT_ZVFBFMIN] || extension_table[EXT_ZVFBFWMA]) && !extension_table['V']) {
    bad_isa_string(str, "'Zvfbfmin/Zvfbfwma' extension requires 'V' extension");
  }

  if (extension_table[EXT_ZFBFMIN] || extension_table[EXT_ZVFBFMIN] || extension_table[EXT_ZFHMIN]) {
    extension_table[EXT_INTERNAL_ZFH_MOVE] = true;
  }

  if (extension_table[EXT_ZFINX] && extension_table['F']) {
    bad_isa_string(str, ("Zfinx/Zdinx/Zhinx{min} extensions conflict with 'F/D/Q/Zfh{min}' extensions"));
  }

  if (extension_table[EXT_ZCF] && !extension_table['F']) {
    bad_isa_string(str, "'Zcf' extension requires 'F' extension");
  }

  if (extension_table[EXT_ZCD] && !extension_table['D']) {
    bad_isa_string(str, "'Zcd' extension requires 'D' extension");
  }

  if ((extension_table[EXT_ZCMP] || extension_table[EXT_ZCMT]) && extension_table[EXT_ZCD]) {
    bad_isa_string(str, "Zcmp' and 'Zcmt' extensions are incompatible with 'Zcd' extension");
  }

  if ((extension_table[EXT_ZCF] || extension_table[EXT_ZCD] || extension_table[EXT_ZCB] ||
       extension_table[EXT_ZCMP] || extension_table[EXT_ZCMT]) && !extension_table[EXT_ZCA]) {
    bad_isa_string(str, "'Zcf/Zcd/Zcb/Zcmp/Zcmt' extensions require 'Zca' extension");
  }

  if (extension_table[EXT_ZACAS] && !extension_table[EXT_ZAAMO]) {
    bad_isa_string(str, "'Zacas' extension requires either the 'A' or the 'Zaamo' extension");
  }

  if (extension_table[EXT_ZABHA] && !extension_table[EXT_ZAAMO]) {
    bad_isa_string(str, "'Zabha' extension requires either the 'A' or the 'Zaamo' extension");
  }

  if (extension_table[EXT_ZAWRS] && !extension_table[EXT_ZALRSC]) {
    bad_isa_string(str, "'Zabha' extension requires either the 'A' or the 'Zalrsc' extension");
  }

  // When SSE is 0, Zicfiss behavior is defined by Zicmop
  if (extension_table[EXT_ZICFISS] && !extension_table[EXT_ZIMOP]) {
    bad_isa_string(str, "'Zicfiss' extension requires 'Zimop' extension");
  }

  if (extension_table[EXT_ZICFISS] && extension_table[EXT_ZCA] &&
      !extension_table[EXT_ZCMOP]) {
    bad_isa_string(str, "'Zicfiss' extension requires 'Zcmop' extension when `Zca` is supported");
  }
#ifdef WORDS_BIGENDIAN
  // Access to the vector registers as element groups is unimplemented on big-endian setups.
  if (extension_table[EXT_ZVKG] || extension_table[EXT_ZVKNHA] || extension_table[EXT_ZVKNHB] ||
      extension_table[EXT_ZVKSED] || extension_table[EXT_ZVKSH]) {
      bad_isa_string(str,
		     "'Zvkg', 'Zvkned', 'Zvknha', 'Zvknhb', 'Zvksed', and 'Zvksh' "
		     "extensions are incompatible with WORDS_BIGENDIAN setups.");
  }
#endif

  if (vlen > 4096) {
    bad_isa_string(str, "Spike does not currently support VLEN > 4096b");
  }

  if ((vlen != 0) ^ (elen != 0)) {
    bad_isa_string(str, "Invalid Zvl/Zve configuration");
  }

  if (extension_table[EXT_ZVFHMIN] && (vlen == 0 || elen == 0 || !zvf)) {
    bad_isa_string(str, "'Zvfhmin' extension requires Zve32f");
  }

  if (extension_table[EXT_ZVFH] && (vlen == 0 || elen == 0 || !zvf || !extension_table[EXT_ZVFHMIN])) {
    bad_isa_string(str, "'Zvfh' extension requires Zve32f and 'Zvfhmin'");
  }

  if (zvd && !extension_table['D'] && elen < 64) {
    bad_isa_string(str, "'ZveXXd' extension requires D");
  }

  if (zvf && !extension_table['F']) {
    bad_isa_string(str, "'ZveXXf' extension requires F");
  }

  std::string lowercase = strtolower(priv);
  bool user = false, supervisor = false;

  if (lowercase == "m")
    ;
  else if (lowercase == "mu")
    user = true;
  else if (lowercase == "msu")
    user = supervisor = true;
  else
    bad_priv_string(priv);

  extension_table['U'] = user;
  extension_table['S'] = supervisor;

  if (extension_table['H'] && !supervisor)
    bad_isa_string(str, "'H' extension requires S mode");

  max_isa = max_xlen == 32 ? reg_t(1) << 30 : reg_t(2) << 62;
  for (unsigned char ch = 'A'; ch <= 'Z'; ch++) {
    if (extension_table[ch])
      max_isa |= 1UL << (ch - 'A');
  }
}
