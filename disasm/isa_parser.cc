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
  unsigned xlens;
  std::vector<unsigned> enables;
  std::vector<const char*> implies;

  // `enables` sets extension_table bits directly
  // `implies` recursively parses and applies other named extensions
  extension_info_t(const char* name,
                   unsigned xlens,
                   std::initializer_list<unsigned> enables = {},
                   std::initializer_list<const char*> implies = {})
    : name(name), xlens(xlens), enables(enables), implies(implies) {}
};

// if every extension in components is set, also set the enable
struct extension_combination_t {
  unsigned enable;
  std::vector<unsigned> components;

  extension_combination_t(unsigned enable,
                          std::initializer_list<unsigned> components = {})
    : enable(enable), components(components) {}
};

// Entries without enables set no extension_table bits directly. This is either
// because no dedicated EXT_* bit exists for them (their full effect is expressed
// through their implies chain), or because their behavior is always present in
// Spike, is currently unmodeled, or is handled by later parser checks.
static const extension_info_t extension_infos[] = {
  {"i", 32 | 64, {'I'}},
  {"e", 32 | 64, {'E'}},
  {"m", 32 | 64, {'M'}},
  {"a", 32 | 64, {'A'}, {"zaamo", "zalrsc"}},
  {"f", 32 | 64, {'F'}},
  {"d", 32 | 64, {'D', 'F'}},
  {"q", 32 | 64, {'Q', 'D', 'F'}},
  {"c", 32 | 64, {'C'}, {"zca"}},
  {"b", 32 | 64, {'B'}, {"zba", "zbb", "zbs"}},
  {"p", 32 | 64, {'P'}},
  {"v", 32 | 64, {'V'}, {"zve64d", "zvl128b"}},
  {"h", 32 | 64, {'H'}},
  // S and U are selected by --priv; implied bits record prerequisites.
  {"s", 32 | 64, {'S', 'U'}},
  {"u", 32 | 64, {'U'}},
  {"sha", 32 | 64, {}, {"h", "ssstateen", "shcounterenw", "shvstvala", "shtvala", "shvstvecd", "shvsatpa", "shgatpa"}},
  // Profile properties imply privilege extensions, but no further features.
  {"ssstateen", 32 | 64, {}, {"s"}},
  {"shcounterenw", 32 | 64, {}, {"h"}},
  {"shvstvala", 32 | 64, {}, {"h"}},
  {"shtvala", 32 | 64, {}, {"h"}},
  {"shvstvecd", 32 | 64, {}, {"h"}},
  {"shvsatpa", 32 | 64, {}, {"h"}},
  {"shgatpa", 32 | 64, {}, {"h"}},
  {"ssccptr", 32 | 64, {}, {"s"}},
  {"sstvecd", 32 | 64, {}, {"s"}},
  {"sstvala", 32 | 64, {}, {"s"}},
  {"sscounterenw", 32 | 64, {}, {"s"}},
  {"ssu64xl", 64, {}, {"s"}},
  {"ssstrict", 32 | 64, {}, {"s"}},
  {"zfh", 32 | 64, {EXT_ZFH}, {"zfhmin"}},
  {"zfhmin", 32 | 64, {'F', EXT_ZFHMIN}},
  {"zvfh", 32 | 64, {EXT_ZVFH}, {"zvfhmin", "zfhmin"}},
  {"zvfhmin", 32 | 64, {EXT_ZVFHMIN}, {"zve32f"}},
  {"zvfbfa", 32 | 64, {EXT_ZVFBFA}, {"zve32f", "zfbfmin"}},
  {"zvfofp4min", 32 | 64, {EXT_ZVFOFP4MIN}, {"zve32f"}},
  {"zvfofp8min", 32 | 64, {EXT_ZVFOFP8MIN}, {"zve32f"}},
  {"zicsr", 32 | 64},
  {"zifencei", 32 | 64},
  {"zihintpause", 32 | 64},
  {"zihintntl", 32 | 64},
  {"ziccid", 32 | 64, {EXT_ZICCID}},
  {"ziccif", 32 | 64},
  {"zve32x", 32 | 64, {}, {"zicsr", "zvl32b"}},
  {"zve32f", 32 | 64, {}, {"zve32x", "f"}},
  {"zve64x", 32 | 64, {}, {"zve32x", "zvl64b"}},
  {"zve64f", 32 | 64, {}, {"zve32f", "zve64x"}},
  {"zve64d", 32 | 64, {}, {"zve64f", "d"}},
  {"zaamo", 32 | 64, {EXT_ZAAMO}},
  {"zalrsc", 32 | 64, {EXT_ZALRSC}},
  {"zacas", 32 | 64, {EXT_ZACAS, EXT_ZAAMO}},
  {"zabha", 32 | 64, {EXT_ZABHA, EXT_ZAAMO}},
  {"zawrs", 32 | 64, {EXT_ZAWRS, EXT_ZALRSC}},
  {"za4rs", 32, {EXT_ZA4RS}, {"zalrsc"}},
  {"za8rs", 32 | 64, {EXT_ZA8RS}, {"zalrsc"}},
  {"za16rs", 32 | 64, {EXT_ZA16RS}, {"zalrsc"}},
  {"za32rs", 32 | 64, {EXT_ZA32RS}, {"zalrsc"}},
  {"za64rs", 32 | 64, {EXT_ZA64RS}, {"zalrsc"}},
  {"za128rs", 32 | 64, {EXT_ZA128RS}, {"zalrsc"}},
  {"za256rs", 32 | 64, {EXT_ZA256RS}, {"zalrsc"}},
  {"za512rs", 32 | 64, {EXT_ZA512RS}, {"zalrsc"}},
  {"zama16b", 32 | 64, {EXT_ZAMA16B}},
  {"zmmul", 32 | 64, {EXT_ZMMUL}},
  {"zba", 32 | 64, {EXT_ZBA}},
  {"zbb", 32 | 64, {EXT_ZBB}},
  {"zbc", 32 | 64, {EXT_ZBC}},
  {"zbs", 32 | 64, {EXT_ZBS}},
  {"zbkb", 32 | 64, {EXT_ZBKB}},
  {"zbkc", 32 | 64, {EXT_ZBKC}},
  {"zbkx", 32 | 64, {EXT_ZBKX}},
  {"zdinx", 32 | 64, {EXT_ZFINX, EXT_ZDINX}},
  {"zfbfmin", 32 | 64, {'F', EXT_ZFBFMIN}},
  {"zfinx", 32 | 64, {EXT_ZFINX}},
  {"zhinx", 32 | 64, {EXT_ZFINX, EXT_ZHINX, EXT_ZHINXMIN}},
  {"zhinxmin", 32 | 64, {EXT_ZFINX, EXT_ZHINXMIN}},
  {"zce", 32 | 64, {EXT_ZCE}, {"zcb", "zcmp", "zcmt"}},
  {"zca", 32 | 64, {EXT_ZCA}},
  {"zcf", 32, {'F', EXT_ZCF}, {"zca"}},
  {"zcb", 32 | 64, {EXT_ZCB}, {"zca"}},
  {"zcd", 32 | 64, {EXT_ZCD, 'F', 'D'}, {"zca"}},
  {"zcmp", 32 | 64, {EXT_ZCMP}, {"zca"}},
  {"zcmt", 32 | 64, {EXT_ZCMT}, {"zca", "zicsr"}},
  {"zibi", 32 | 64, {EXT_ZIBI}},
  {"zk", 32 | 64, {EXT_ZBKB, EXT_ZBKC, EXT_ZBKX, EXT_ZKND, EXT_ZKNE, EXT_ZKNH, EXT_ZKR}},
  {"zkn", 32 | 64, {EXT_ZBKB, EXT_ZBKC, EXT_ZBKX, EXT_ZKND, EXT_ZKNE, EXT_ZKNH}},
  {"zknd", 32 | 64, {EXT_ZKND}},
  {"zkne", 32 | 64, {EXT_ZKNE}},
  {"zknh", 32 | 64, {EXT_ZKNH}},
  {"zks", 32 | 64, {EXT_ZBKB, EXT_ZBKC, EXT_ZBKX, EXT_ZKSED, EXT_ZKSH}},
  {"zksed", 32 | 64, {EXT_ZKSED}},
  {"zksh", 32 | 64, {EXT_ZKSH}},
  {"zkr", 32 | 64, {EXT_ZKR}},
  {"zic64b", 32 | 64},
  {"zkt", 32 | 64},
  {"smepmp", 32 | 64, {EXT_SMEPMP}},
  {"smpmpdeleg", 32 | 64, {EXT_SMPMPDELEG}},
  {"sspmp", 32 | 64, {EXT_SSPMP, EXT_SMCSRIND, EXT_SSCSRIND}, {"smpmpdeleg", "s"}},
  {"sspmpen", 32 | 64, {EXT_SSPMPEN}, {"s"}},
  {"smstateen", 32 | 64, {EXT_SMSTATEEN}},
  {"smpmpmt", 32 | 64, {EXT_SMPMPMT}},
  {"smrnmi", 32 | 64, {EXT_SMRNMI}},
  {"sscofpmf", 32 | 64, {EXT_SSCOFPMF}, {"s"}},
  {"svadu", 32 | 64, {EXT_SVADU}, {"s"}},
  {"svade", 32 | 64, {EXT_SVADE}, {"s"}},
  {"svbare", 32 | 64, {}, {"s"}},
  {"svnapot", 32 | 64, {EXT_SVNAPOT}, {"s"}},
  {"svpbmt", 32 | 64, {EXT_SVPBMT}, {"s"}},
  {"svinval", 32 | 64, {EXT_SVINVAL}, {"s"}},
  {"svukte", 64, {EXT_SVUKTE}, {"s"}},
  {"zfa", 32 | 64, {EXT_ZFA}},
  {"zicbom", 32 | 64, {EXT_ZICBOM}},
  {"zicboz", 32 | 64, {EXT_ZICBOZ}},
  {"zicbop", 32 | 64},
  {"ziccamoa", 32 | 64},
  {"ziccamoc", 32 | 64},
  {"zicclsm", 32 | 64, {EXT_ZICCLSM}},
  {"ziccrse", 32 | 64},
  {"ztso", 32 | 64},
  {"zicntr", 32 | 64, {EXT_ZICNTR}},
  {"zicond", 32 | 64, {EXT_ZICOND}},
  {"zihpm", 32 | 64, {EXT_ZIHPM}},
  {"zilsd", 32, {EXT_ZILSD}},
  {"zclsd", 32, {EXT_ZCLSD}, {"zilsd", "zca"}},
  {"zvabd", 32 | 64, {EXT_ZVABD}},
  {"zvkb", 32 | 64, {EXT_ZVKB}},
  {"zvbb", 32 | 64, {EXT_ZVKB, EXT_ZVBB}},
  {"zvbc", 32 | 64, {EXT_ZVBC}},
  {"zvfbfmin", 32 | 64, {EXT_ZVFBFMIN}, {"zve32f"}},
  {"zvfbfwma", 32 | 64, {EXT_ZVFBFWMA}, {"zfbfmin", "zvfbfmin"}},
  {"zvkg", 32 | 64, {EXT_ZVKG}},
  {"zvkn", 32 | 64, {EXT_ZVKB, EXT_ZVKNED, EXT_ZVKNHB}},
  {"zvknc", 32 | 64, {EXT_ZVKB, EXT_ZVBC, EXT_ZVKNED, EXT_ZVKNHB}},
  {"zvkng", 32 | 64, {EXT_ZVKB, EXT_ZVKG, EXT_ZVKNED, EXT_ZVKNHB}},
  {"zvkned", 32 | 64, {EXT_ZVKNED}},
  {"zvknha", 32 | 64, {EXT_ZVKNHA}},
  {"zvknhb", 32 | 64, {EXT_ZVKNHB}},
  {"zvks", 32 | 64, {EXT_ZVKB, EXT_ZVKSED, EXT_ZVKSH}},
  {"zvksc", 32 | 64, {EXT_ZVKB, EXT_ZVBC, EXT_ZVKSED, EXT_ZVKSH}},
  {"zvksg", 32 | 64, {EXT_ZVKB, EXT_ZVKG, EXT_ZVKSED, EXT_ZVKSH}},
  {"zvksed", 32 | 64, {EXT_ZVKSED}},
  {"zvksh", 32 | 64, {EXT_ZVKSH}},
  {"zvdot4a", 32 | 64, {EXT_ZVDOT4A}},
  {"zvqwbdota8i", 32 | 64, {EXT_ZVQWBDOTA8I}},
  {"zvqwbdota16i", 32 | 64, {EXT_ZVQWBDOTA16I}},
  {"zvfqwbdota8f", 32 | 64, {EXT_ZVFQWBDOTA8F}},
  {"zvfwbdota16bf", 32 | 64, {EXT_ZVFWBDOTA16BF}},
  {"zvfbdota32f", 32 | 64, {EXT_ZVFBDOTA32F}},
  {"zvqwdota8i", 32 | 64, {EXT_ZVQWDOTA8I}},
  {"zvqwdota16i", 32 | 64, {EXT_ZVQWDOTA16I}},
  {"zvfqwdota8f", 32 | 64, {EXT_ZVFQWDOTA8F}},
  {"zvfwdota16bf", 32 | 64, {EXT_ZVFWDOTA16BF}},
  {"zvkt", 32 | 64},
  {"zvtbase", 32 | 64, {EXT_ZVTBASE}, {"zve32x"}},
  {"zvt8t", 32 | 64, {EXT_ZVT8T}, {"zvtbase"}},
  {"zvt16t", 32 | 64, {EXT_ZVT16T}, {"zvt8t"}},
  {"zvt32t", 32 | 64, {EXT_ZVT32T}, {"zvt16t"}},
  {"zvt64t", 32 | 64, {EXT_ZVT64T}, {"zvt32t"}},
  {"zvt128t", 32 | 64, {EXT_ZVT128T}, {"zvt64t"}},
  {"zvt256t", 32 | 64, {EXT_ZVT256T}, {"zvt128t"}},
  {"zvt8e", 32 | 64, {EXT_ZVT8E}, {"zvtbase"}},
  {"zvt16e", 32 | 64, {EXT_ZVT16E}, {"zvtbase"}},
  {"zvt64e", 32 | 64, {EXT_ZVT64E}, {"zvtbase", "zve64x"}},
  {"zvti8i32mm", 32 | 64, {EXT_ZVTI8I32MM}, {"zvt8e"}},
  {"zvtofp8fmm", 32 | 64, {EXT_ZVTOFP8FMM}, {"zvt8e", "zve32f"}},
  {"zvtfp16fmm", 32 | 64, {EXT_ZVTFP16FMM}, {"zvt16e", "zve32f"}},
  {"zvtbf16fmm", 32 | 64, {EXT_ZVTBF16FMM}, {"zvt16e", "zve32f"}},
  {"zvtfmm", 32 | 64, {EXT_ZVTFMM}, {"zvtbase", "zve32f"}},
  {"zvtdmm", 32 | 64, {EXT_ZVTDMM}, {"zvt64e", "zve64d"}},
  {"zvzip", 32 | 64, {EXT_ZVZIP}},
  {"zilx", 32 | 64, {EXT_ZILX}},
  {"sstc", 32 | 64, {EXT_SSTC, EXT_ZICNTR}, {"s"}},
  {"smcsrind", 32 | 64, {EXT_SMCSRIND}},
  {"sscsrind", 32 | 64, {EXT_SSCSRIND}, {"s"}},
  {"smcntrpmf", 32 | 64, {EXT_SMCNTRPMF}},
  {"smcdeleg", 32 | 64, {EXT_SMCDELEG}},
  {"ssccfg", 32 | 64, {EXT_SSCCFG}, {"s"}},
  {"zimop", 32 | 64, {EXT_ZIMOP}},
  {"zcmop", 32 | 64, {EXT_ZCMOP}, {"zca"}},
  {"zalasr", 32 | 64, {EXT_ZALASR}},
  {"ssqosid", 32 | 64, {EXT_SSQOSID}, {"s"}},
  {"zicfilp", 32 | 64, {EXT_ZICFILP}},
  {"zicfiss", 32 | 64, {EXT_ZICFISS}, {"zaamo", "zimop"}},
  {"smmpm", 32 | 64, {EXT_SMMPM}},
  {"smnpm", 32 | 64, {EXT_SMNPM}},
  {"ssnpm", 32 | 64, {EXT_SSNPM}, {"s"}},
  {"sspm", 32 | 64, {}, {"s"}},
  {"supm", 32 | 64, {}, {"u"}},
  {"ssdbltrp", 32 | 64, {EXT_SSDBLTRP}, {"s"}},
  {"smdbltrp", 32 | 64, {EXT_SMDBLTRP}},
  {"smaia", 32 | 64, {EXT_SMAIA, EXT_SSAIA, EXT_SMCSRIND, EXT_SSCSRIND}},
  {"ssaia", 32 | 64, {EXT_SSAIA, EXT_SSCSRIND}, {"s"}},
  {"svvptc", 32 | 64, {}, {"s"}},
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
    if (!(info->xlens & max_xlen))
      bad_isa_string(str, ("'" + ext_str + "' is incompatible with XLEN=" + std::to_string(max_xlen)).c_str());
    for (const auto ext : info->enables) {
      extension_table[ext] = true;
    }
    for (const auto implied : info->implies) {
      add_extension(implied, str);
    }
    if (ext_str.starts_with("zve"))
      apply_zve_properties(ext_str, str);
  } else if (ext_str.starts_with("zvl")) {
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

  if (isa_string.starts_with("rv32"))
    max_xlen = 32;
  else if (isa_string.starts_with("rv64"))
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

  // Do not silently override the privilege modes selected by --priv.
  if (extension_table['S'] && !supervisor)
    bad_isa_string(str, "ISA extensions require S mode");
  if (extension_table['U'] && !user)
    bad_isa_string(str, "ISA extensions require U mode");

  extension_table['U'] = user;
  extension_table['S'] = supervisor;

  if (extension_table['H'] && !supervisor)
    bad_isa_string(str, "'H' extension requires S mode");

  max_isa = max_xlen == 32 ? reg_t(1) << 30 : reg_t(2) << 62;
  for (unsigned char ch = 'A'; ch <= 'Z'; ch++) {
    if (extension_table[ch])
      max_isa |= 1UL << (ch - 'A');
  }

  if (extension_table[EXT_SSPMP] && !supervisor)
    bad_isa_string(str, "'SPMP' extension requires S mode");
}

reg_t isa_parser_t::get_te() const
{
  return
    extension_enabled(EXT_ZVT256T) ? 256 :
    extension_enabled(EXT_ZVT128T) ? 128 :
    extension_enabled(EXT_ZVT64T) ? 64 :
    extension_enabled(EXT_ZVT32T) ? 32 :
    extension_enabled(EXT_ZVT16T) ? 16 :
    extension_enabled(EXT_ZVT8T) ? 8 :
    extension_enabled(EXT_ZVTBASE) ? 4 : 0;
}
