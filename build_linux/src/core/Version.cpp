#include "i3070/core/Version.hpp"

namespace i3070 {
namespace core {

static const std::string VERSION_FULL = "0.0.0.6.ac181a7";
static const std::string BUILD_HASH = "ac181a7";

std::string get_version() {
    return VERSION_FULL;
}

std::string get_build_hash() {
    return BUILD_HASH;
}

} // namespace core
} // namespace i3070

extern "C" {
    const char* get_version_c() {
        return i3070::core::VERSION_FULL.c_str();
    }

    const char* get_build_hash_c() {
        return i3070::core::BUILD_HASH.c_str();
    }
}
