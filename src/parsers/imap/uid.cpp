#include "parsers/imap/uid.h"
#include <loglibrary.h>

#include <regex>

Uid::Uid(std::string s)
{
    std::regex uidRegex(UID_REGEX);
    std::smatch match;
    if (std::regex_search(s, match, uidRegex)){
        success = true;
        try {
            uid = stoi(match[1]);
        } catch(std::exception e){
            ERROR("Could not parse UID! Error: {}, Response: {}", *e.what(), s);
            success = false;
        }
    } else {
        ERROR("Could not parse UID! Response: {}", s);
        success = false;
    }
}
