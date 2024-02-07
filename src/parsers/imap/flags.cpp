#include "parsers/imap/flags.h"
#include "utils.h"
#include <loglibrary.h>
#include <regex>

Flags::Flags(std::string s)
{
    success = true;
    std::regex flagsRegex(MESSAGE_FLAGS_REGEX);
    std::smatch match;

    if (!std::regex_search(s, match, flagsRegex)){
        success = false;
        ERROR("Could not parse flags: {}", s);
    } else {
        auto f = match[1].str();
        flags = splitString(f);
    }
}
