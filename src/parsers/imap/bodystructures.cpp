#include "parsers/imap/bodystructures.h"
#include <regex>
#include <loglibrary.h>

BodyStructures::BodyStructures(std::string s)
{
    std::regex bodyStructureRegex(BODYSTRUCTURE_REGEX);
    std::smatch match;
    std::string bodyStructure;


    while (std::regex_search(s, match, bodyStructureRegex)){
        bodyStructure = match[1];
        if (bodyStructure[0] == '(')
            bodyStructure = bodyStructure.substr(1);
        BodyStructure tmp(bodyStructure);

        bodyStructures.push_back(tmp);
        s = match.suffix();
    }

    LOG("Size before deletion: {}", bodyStructures.size());
    bodyStructures.erase(std::remove_if(bodyStructures.begin(), bodyStructures.end(), [](BodyStructure bs){return !bs.isSuccess();}), bodyStructures.end());
    LOG("Size after deletion: {}", bodyStructures.size());
}
