#include "isa_parser.h"
#include <cstring>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace {

static std::string strtolower(const char* str)
{
  std::string res(str);
  for (char &c : res)
    c = std::tolower(c);
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

struct extension_info_t {
  const char* name;
  std::vector<unsigned> enables;
  std::vector<const char*> implies;
  unsigned required_xlen = 0;
  const char* required_xlen_msg = nullptr;

  extension_info_t(const char* name,
                   std::initializer_list<unsigned> enables = {},
                   std::initializer_list<const char*> implies = {},
                   unsigned required_xlen = 0,
                   const char* required_xlen_msg = nullptr)
    : name(name), enables(enables), implies(implies),
      required_xlen(required_xlen), required_xlen_msg(required_xlen_msg) {}
};

struct extension_combination_t {
  unsigned enable;
  std::vector<unsigned> components;

  extension_combination_t(unsigned enable,
                          std::initializer_list<unsigned> components = {})
    : enable(enable), components(components) {}
};

// Entries without enables are accepted ISA names whose behavior is always present
// in Spike, currently unmodeled, or handled by later parser checks.
static const extension_info_t extension_infos[] = {
  {"i", {'I'}},
  {"e", {'E'}},
  {"m", {'M'}},
  {"a", {'A'}, {"zaamo", "zalrsc"}},
  {"f", {'F'}},
  {"d", {'D', 'F'}},
  {"q", {'Q', 'D', 'F'}},
  {"c", {'C'}, {"zca"}},
  {"b", {'B'}, {"zba", "zbb", "zbs"}},
  {"p", {'P'}},
  {"v", {'V'}, {"zve64d", "zvl128b"}},
  {"h", {'H'}},
  {"zfh", {EXT_ZFH}, {"zfhmin"}},
  {"zfhmin", {'F', EXT_ZFHMIN}},
  {"zvfh", {EXT_ZVFH}, {"zvfhmin", "zfhmin"}},
  {"zvfhmin", {EXT_ZVFHMIN}, {"zve32f"}},
  {"zvfbfa", {EXT_ZVFBFA}, {"zve32f", "zfbfmin"}},
  {"zvfofp4min", {EXT_ZVFOFP4MIN}, {"zve32f"}},
  {"zvfofp8min", {EXT_ZVFOFP8MIN}, {"zve32f"}},
  {"zicsr"},
  {"zifencei"},
  {"zihintpause"},
  {"zihintntl"},
  {"ziccid", {EXT_ZICCID}},
  {"ziccif"},
  {"zve32x", {}, {"zicsr", "zvl32b"}},
  {"zve32f", {}, {"zve32x", "f"}},
  {"zve64x", {}, {"zve32x", "zvl64b"}},
  {"zve64f", {}, {"zve32f", "zve64x"}},
  {"zve64d", {}, {"zve64f", "d"}},
  {"zaamo", {EXT_ZAAMO}},
  {"zalrsc", {EXT_ZALRSC}},
  {"zacas", {EXT_ZACAS, EXT_ZAAMO}},
  {"zabha", {EXT_ZABHA, EXT_ZAAMO}},
  {"zawrs", {EXT_ZAWRS, EXT_ZALRSC}},
  {"zama16b", {EXT_ZAMA16B}},
  {"zmmul", {EXT_ZMMUL}},
  {"zba", {EXT_ZBA}},
  {"zbb", {EXT_ZBB}},
  {"zbc", {EXT_ZBC}},
  {"zbs", {EXT_ZBS}},
  {"zbkb", {EXT_ZBKB}},
  {"zbkc", {EXT_ZBKC}},
  {"zbkx", {EXT_ZBKX}},
  {"zdinx", {EXT_ZFINX, EXT_ZDINX}},
  {"zfbfmin", {'F', EXT_ZFBFMIN}},
  {"zfinx", {EXT_ZFINX}},
  {"zhinx", {EXT_ZFINX, EXT_ZHINX, EXT_ZHINXMIN}},
  {"zhinxmin", {EXT_ZFINX, EXT_ZHINXMIN}},
  {"zce", {EXT_ZCE}, {"zcb", "zcmp", "zcmt"}},
  {"zca", {EXT_ZCA}},
  {"zcf", {'F', EXT_ZCF}, {"zca"}, 32, "'Zcf' requires RV32"},
  {"zcb", {EXT_ZCB}, {"zca"}},
  {"zcd", {EXT_ZCD, 'F', 'D'}, {"zca"}},
  {"zcmp", {EXT_ZCMP}, {"zca"}},
  {"zcmt", {EXT_ZCMT}, {"zca", "zicsr"}},
  {"zibi", {EXT_ZIBI}},
  {"zk", {EXT_ZBKB, EXT_ZBKC, EXT_ZBKX, EXT_ZKND, EXT_ZKNE, EXT_ZKNH, EXT_ZKR}},
  {"zkn", {EXT_ZBKB, EXT_ZBKC, EXT_ZBKX, EXT_ZKND, EXT_ZKNE, EXT_ZKNH}},
  {"zknd", {EXT_ZKND}},
  {"zkne", {EXT_ZKNE}},
  {"zknh", {EXT_ZKNH}},
  {"zks", {EXT_ZBKB, EXT_ZBKC, EXT_ZBKX, EXT_ZKSED, EXT_ZKSH}},
  {"zksed", {EXT_ZKSED}},
  {"zksh", {EXT_ZKSH}},
  {"zkr", {EXT_ZKR}},
  {"zkt"},
  {"smepmp", {EXT_SMEPMP}},
  {"smstateen", {EXT_SMSTATEEN}},
  {"smpmpmt", {EXT_SMPMPMT}},
  {"smrnmi", {EXT_SMRNMI}},
  {"sscofpmf", {EXT_SSCOFPMF}},
  {"svadu", {EXT_SVADU}},
  {"svade", {EXT_SVADE}},
  {"svnapot", {EXT_SVNAPOT}},
  {"svpbmt", {EXT_SVPBMT}},
  {"svinval", {EXT_SVINVAL}},
  {"svukte", {EXT_SVUKTE}, {}, 64, "'svukte' requires RV64"},
  {"zfa", {EXT_ZFA}},
  {"zicbom", {EXT_ZICBOM}},
  {"zicboz", {EXT_ZICBOZ}},
  {"zicbop"},
  {"zicclsm", {EXT_ZICCLSM}},
  {"zicntr", {EXT_ZICNTR}},
  {"zicond", {EXT_ZICOND}},
  {"zihpm", {EXT_ZIHPM}},
  {"zilsd", {EXT_ZILSD}, {}, 32, "'Zilsd' requires RV32"},
  {"zclsd", {EXT_ZCLSD}, {"zilsd", "zca"}, 32, "'Zclsd' requires RV32"},
  {"zvabd", {EXT_ZVABD}},
  {"zvkb", {EXT_ZVKB}},
  {"zvbb", {EXT_ZVKB, EXT_ZVBB}},
  {"zvbc", {EXT_ZVBC}},
  {"zvfbfmin", {EXT_ZVFBFMIN}, {"zve32f"}},
  {"zvfbfwma", {EXT_ZVFBFWMA}, {"zfbfmin", "zvfbfmin"}},
  {"zvkg", {EXT_ZVKG}},
  {"zvkn", {EXT_ZVKB, EXT_ZVKNED, EXT_ZVKNHB}},
  {"zvknc", {EXT_ZVKB, EXT_ZVBC, EXT_ZVKNED, EXT_ZVKNHB}},
  {"zvkng", {EXT_ZVKB, EXT_ZVKG, EXT_ZVKNED, EXT_ZVKNHB}},
  {"zvkned", {EXT_ZVKNED}},
  {"zvknha", {EXT_ZVKNHA}},
  {"zvknhb", {EXT_ZVKNHB}},
  {"zvks", {EXT_ZVKB, EXT_ZVKSED, EXT_ZVKSH}},
  {"zvksc", {EXT_ZVKB, EXT_ZVBC, EXT_ZVKSED, EXT_ZVKSH}},
  {"zvksg", {EXT_ZVKB, EXT_ZVKG, EXT_ZVKSED, EXT_ZVKSH}},
  {"zvksed", {EXT_ZVKSED}},
  {"zvksh", {EXT_ZVKSH}},
  {"zvqdotq", {EXT_ZVQDOTQ}},
  {"zvqbdot8i", {EXT_ZVQBDOT8I}},
  {"zvqbdot16i", {EXT_ZVQBDOT16I}},
  {"zvfqbdot8f", {EXT_ZVFQBDOT8F}},
  {"zvfwbdot16bf", {EXT_ZVFWBDOT16BF}},
  {"zvfbdot32f", {EXT_ZVFBDOT32F}},
  {"zvqldot8i", {EXT_ZVQLDOT8I}},
  {"zvqldot16i", {EXT_ZVQLDOT16I}},
  {"zvfqldot8f", {EXT_ZVFQLDOT8F}},
  {"zvfwldot16bf", {EXT_ZVFWLDOT16BF}},
  {"zvkt"},
  {"zvzip", {EXT_ZVZIP}},
  {"sstc", {EXT_SSTC}},
  {"smcsrind", {EXT_SMCSRIND}},
  {"sscsrind", {EXT_SSCSRIND}},
  {"smcntrpmf", {EXT_SMCNTRPMF}},
  {"smcdeleg", {EXT_SMCDELEG}},
  {"ssccfg", {EXT_SSCCFG}},
  {"zimop", {EXT_ZIMOP}},
  {"zcmop", {EXT_ZCMOP}, {"zca"}},
  {"zalasr", {EXT_ZALASR}},
  {"ssqosid", {EXT_SSQOSID}},
  {"zicfilp", {EXT_ZICFILP}},
  {"zicfiss", {EXT_ZICFISS}, {"zaamo", "zimop"}},
  {"smmpm", {EXT_SMMPM}},
  {"smnpm", {EXT_SMNPM}},
  {"ssnpm", {EXT_SSNPM}},
  {"ssdbltrp", {EXT_SSDBLTRP}},
  {"smdbltrp", {EXT_SMDBLTRP}},
  {"smaia", {EXT_SMAIA, EXT_SSAIA, EXT_SMCSRIND, EXT_SSCSRIND}},
  {"ssaia", {EXT_SSAIA, EXT_SSCSRIND}},
  {"svvptc"},
};

static const extension_combination_t extension_combinations[] = {
  {'A', {EXT_ZAAMO, EXT_ZALRSC}},
  {'B', {EXT_ZBA, EXT_ZBB, EXT_ZBS}},
  {EXT_ZCD, {'C', 'D'}},
  {EXT_ZCMOP, {EXT_ZICFISS, EXT_ZCA}},
};

static const extension_info_t* find_extension_info(const std::string& ext)
{
  for (const auto& info : extension_infos)
    if (ext == info.name)
      return &info;
  return nullptr;
}

static void apply_extension_combinations(std::bitset<NUM_ISA_EXTENSIONS>& extension_table)
{
  for (const auto& combination : extension_combinations) {
    if (combination.components.empty())
      continue;
    bool has_all_components = true;
    for (const auto component : combination.components) {
      has_all_components &= extension_table[component];
    }
    if (has_all_components)
      extension_table[combination.enable] = true;
  }
}

} // namespace

void isa_parser_t::apply_zve_properties(const std::string& ext_str, const char* str)
{
  if (ext_str.size() != 6)
    bad_isa_string(str, ("Invalid Zve string: " + ext_str).c_str());

  reg_t new_elen;
  try {
    new_elen = safe_stoul(ext_str.substr(3, ext_str.size() - 4));
  } catch (std::logic_error& e) {
    new_elen = 0;
  }

  switch (ext_str.back()) {
    case 'd':
      zvd |= true;
      [[fallthrough]];
    case 'f':
      zvf |= true;
      break;
    case 'x':
      break;
    default:
      new_elen = 0;
  }

  if (new_elen != 32 && new_elen != 64)
    bad_isa_string(str, ("Invalid Zve string: " + ext_str).c_str());

  elen = std::max(elen, new_elen);
  vlen = std::max(vlen, new_elen);
}

void isa_parser_t::add_extension(const std::string& ext_str, const char* str)
{
  if (const auto* info = find_extension_info(ext_str)) {
    if (info->required_xlen && max_xlen != info->required_xlen)
      bad_isa_string(str, info->required_xlen_msg);
    for (const auto ext : info->enables) {
      extension_table[ext] = true;
    }
    for (const auto implied : info->implies) {
      add_extension(implied, str);
    }
    if (ext_str.substr(0, 3) == "zve")
      apply_zve_properties(ext_str, str);
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
  } else if (!ext_str.empty() && ext_str[0] == 'x') {
    extension_table['X'] = true;
    if (ext_str.size() == 1) {
      bad_isa_string(str, "single 'X' is not a proper name");
    } else if (ext_str != "xdummy") {
      extensions.insert(ext_str.substr(1));
    }
  } else {
    bad_isa_string(str, ("unsupported extension: " + ext_str).c_str());
  }
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

  if (isa_string[4] == 'g') {
    // G = IMAFD_Zicsr_Zifencei, but Spike includes the latter two
    // unconditionally, so they need not be explicitly added here.
    isa_string = isa_string.substr(0, 4) + "imafd" + isa_string.substr(5);
  } else if (isa_string[4] != 'i' && isa_string[4] != 'e') {
    bad_isa_string(str, ("'" + isa_string.substr(0, 4) + "' must be followed by I, E, or G").c_str());
  }
  add_extension(std::string(1, isa_string[4]), str);

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

    add_extension(std::string(1, *p), str);
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
    p = end;
    add_extension(ext_str, str);
  }
  if (*p) {
    bad_isa_string(str, ("can't parse: " + std::string(p)).c_str());
  }

  apply_extension_combinations(extension_table);

  if (extension_table[EXT_ZFBFMIN] || extension_table[EXT_ZFHMIN]) {
    extension_table[EXT_INTERNAL_ZFH_MOVE] = true;
  }

  if (extension_table['C'] || extension_table[EXT_ZCE]) {
    if (extension_table['F'] && max_xlen == 32)
      extension_table[EXT_ZCF] = true;
  }

  if (extension_table[EXT_ZCLSD] && extension_table[EXT_ZCF]) {
    bad_isa_string(str, "'Zclsd' extension conflicts with 'Zcf' extensions");
  }

  if (extension_table[EXT_ZFINX] && extension_table['F']) {
    bad_isa_string(str, ("Zfinx/Zdinx/Zhinx{min} extensions conflict with 'F/D/Q/Zfh{min}' extensions"));
  }

  if ((extension_table[EXT_ZCMP] || extension_table[EXT_ZCMT]) && extension_table[EXT_ZCD]) {
    bad_isa_string(str, "Zcmp' and 'Zcmt' extensions are incompatible with 'Zcd' extension");
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
    bad_isa_string(str, "Spike does not support VLEN > 4096");
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
