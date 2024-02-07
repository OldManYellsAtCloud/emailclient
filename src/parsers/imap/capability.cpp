#include "parsers/imap/capability.h"
#include "utils.h"

Capability::Capability(std::string s)
{
    auto start = s.find(CAPABILITY_STRING) + 11;
    if (start != std::string::npos) {
        capabilities = splitString(s.substr(start));
        success = true;
    } else {
        ERROR("Could not parse capabilities string: {}", s);
        success = false;
    }
}
