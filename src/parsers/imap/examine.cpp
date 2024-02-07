#include "parsers/imap/examine.h"
#include "utils.h"
#include <regex>

int Examine::parseInt(std::string s)
{
    try {
        return stoi(s);
    } catch (std::exception e) {
        ERROR("Could not parse string as int: {}, error: {}", s, e.what());
        return -1;
    }
}

Examine::Examine(std::string s) {
    success = true;
    std::regex flagsRegex(FLAGS_REGEX);
    std::regex permanentFlagsRegex(PERMANENT_FLAGS_REGEX);
    std::regex existsRegex(EXISTS_REGEX);
    std::regex recentRegex(RECENT_REGEX);
    std::regex uidNextRegex(UIDNEXT_REGEX);
    std::regex uidValidityRegex(UIDVALIDITY_REGEX);

    std::smatch match;
    auto res = std::regex_search(s, match, flagsRegex);
    LOG("Match size: {}, res: {}", match.size(), res);

    std::string extractedFlags;
    if (match.size() > 1) {
        LOG("Flags found: {}", match[1].str());
        extractedFlags = match[1];
    }

    std::regex_search(s, match, permanentFlagsRegex);
    std::string extractedPermanentFlags;
    if (match.size() > 1 && !match[1].str().empty())
        extractedPermanentFlags = match[1];

    std::regex_search(s, match, existsRegex);
    if (match.size() > 1 && !match[1].str().empty()){
        exists = parseInt(match[1]);
        if (exists < 0)
            success = false;
    }

    std::regex_search(s, match, recentRegex);
    if (match.size() > 1 && !match[1].str().empty()){
        recent = parseInt(match[1]);
        if (recent < 0)
            success = false;
    }

    std::regex_search(s, match, uidNextRegex);
    if (match.size() > 1 && !match[1].str().empty()){
        uidNext = parseInt(match[1]);
        if (uidNext < 0)
            success = false;
    }

    std::regex_search(s, match, uidValidityRegex);
    if (match.size() > 1 && !match[1].str().empty()){
        uidValidity = parseInt(match[1]);
        if (uidValidity < 0)
            success = false;
    }

    flags = splitString(extractedFlags);
    permanentFlags = splitString(extractedPermanentFlags);
}
