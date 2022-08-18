#include "isa_parser.h"
#include "extension.h"

static std::string strtolower(const char* str)
{
  std::string res;
  for (const char *r = str; *r; r++)
    res += std::tolower(*r);
  return res;
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
  : extension_table(256, false)
{
  isa_string = strtolower(str);
  const char* all_subsets = "mafdqchpv";

  max_isa = reg_t(2) << 62;
  // enable zicntr and zihpm unconditionally for backward compatibility
  extension_table[EXT_ZICNTR] = true;
  extension_table[EXT_ZIHPM] = true;

  if (isa_string.compare(0, 4, "rv32") == 0)
    max_xlen = 32, max_isa = reg_t(1) << 30;
  else if (isa_string.compare(0, 4, "rv64") == 0)
    max_xlen = 64;
  else
    bad_isa_string(str, "ISA strings must begin with RV32 or RV64");

  switch (isa_string[4]) {
    case 'g':
      // G = IMAFD_Zicsr_Zifencei, but Spike includes the latter two
      // unconditionally, so they need not be explicitly added here.
      isa_string = isa_string.substr(0, 4) + "imafd" + isa_string.substr(5);
      // Fall through
    case 'i':
      max_isa |= 1L << ('i' - 'a');
      break;

    case 'e':
      max_isa |= 1L << ('e' - 'a');
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
      case 'p': extension_table[EXT_ZBPBO] = true;
                extension_table[EXT_ZPN] = true;
                extension_table[EXT_ZPSFOPERAND] = true;
                extension_table[EXT_ZMMUL] = true; break;
      case 'v': // even rv32iv implies double float
      case 'q': max_isa |= 1L << ('d' - 'a');
                // Fall through
      case 'd': max_isa |= 1L << ('f' - 'a');
    }
    max_isa |= 1L << (*p - 'a');
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
      if (!((max_isa >> ('f' - 'a')) & 1))
        bad_isa_string(str, ("'" + ext_str + "' extension requires 'F'").c_str());
      extension_table[EXT_ZFHMIN] = true;
      if (ext_str == "zfh")
        extension_table[EXT_ZFH] = true;
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
    } else if (ext_str == "zfinx") {
      extension_table[EXT_ZFINX] = true;
    } else if (ext_str == "zhinx") {
      extension_table[EXT_ZFINX] = true;
      extension_table[EXT_ZHINX] = true;
      extension_table[EXT_ZHINXMIN] = true;
    } else if (ext_str == "zhinxmin") {
      extension_table[EXT_ZFINX] = true;
      extension_table[EXT_ZHINXMIN] = true;
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
    } else if (ext_str == "svnapot") {
      extension_table[EXT_SVNAPOT] = true;
    } else if (ext_str == "svpbmt") {
      extension_table[EXT_SVPBMT] = true;
    } else if (ext_str == "svinval") {
      extension_table[EXT_SVINVAL] = true;
    } else if (ext_str == "zicbom") {
      extension_table[EXT_ZICBOM] = true;
    } else if (ext_str == "zicboz") {
      extension_table[EXT_ZICBOZ] = true;
    } else if (ext_str == "zicbop") {
    } else if (ext_str == "zicntr") {
    } else if (ext_str == "zihpm") {
    } else if (ext_str == "sstc") {
        extension_table[EXT_SSTC] = true;
    } else if (ext_str[0] == 'x') {
      max_isa |= 1L << ('x' - 'a');
      extension_table[toupper('x')] = true;
      if (ext_str == "xbitmanip") {
        extension_table[EXT_XZBP] = true;
        extension_table[EXT_XZBS] = true;
        extension_table[EXT_XZBE] = true;
        extension_table[EXT_XZBF] = true;
        extension_table[EXT_XZBC] = true;
        extension_table[EXT_XZBM] = true;
        extension_table[EXT_XZBR] = true;
        extension_table[EXT_XZBT] = true;
      } else if (ext_str == "xzbp") {
        extension_table[EXT_XZBP] = true;
      } else if (ext_str == "xzbs") {
        extension_table[EXT_XZBS] = true;
      } else if (ext_str == "xzbe") {
        extension_table[EXT_XZBE] = true;
      } else if (ext_str == "xzbf") {
        extension_table[EXT_XZBF] = true;
      } else if (ext_str == "xzbc") {
        extension_table[EXT_XZBC] = true;
      } else if (ext_str == "xzbm") {
        extension_table[EXT_XZBM] = true;
      } else if (ext_str == "xzbr") {
        extension_table[EXT_XZBR] = true;
      } else if (ext_str == "xzbt") {
        extension_table[EXT_XZBT] = true;
      } else if (ext_str.size() == 1) {
        bad_isa_string(str, "single 'X' is not a proper name");
      } else if (ext_str != "xdummy") {
         extension_t* x = find_extension(ext_str.substr(1).c_str())();
         if (!extensions.insert(std::make_pair(x->name(), x)).second) {
           fprintf(stderr, "extensions must have unique names (got two named \"%s\"!)\n", x->name());
           abort();
         }
      }
    } else {
      bad_isa_string(str, ("unsupported extension: " + ext_str).c_str());
    }
    p = end;
  }
  if (*p) {
    bad_isa_string(str, ("can't parse: " + std::string(p)).c_str());
  }

  if (extension_table[EXT_ZFINX] && ((max_isa >> ('f' - 'a')) & 1)) {
    bad_isa_string(str, ("Zfinx/ZDinx/Zhinx{min} extensions conflict with Base 'F/D/Q/Zfh{min}' extensions"));
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

  if (user) {
    max_isa |= reg_t(user) << ('u' - 'a');
    extension_table['U'] = true;
  }

  if (supervisor) {
    max_isa |= reg_t(supervisor) << ('s' - 'a');
    extension_table['S'] = true;
  }

  if (((max_isa >> ('h' - 'a')) & 1) && !supervisor)
    bad_isa_string(str, "'H' extension requires S mode");
}
