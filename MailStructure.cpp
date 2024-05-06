#include "MailStructure.h"
#include <regex>
#include <loglibrary.h>


/*
 *
 * The BodyStructure response looks like this:
 * "* 3701 FETCH (UID 6592 BODYSTRUCTURE (("TEXT" "PLAIN" ("CHARSET" "UTF-8") NIL NIL "7BIT" 143 6 NIL NIL NIL)\
 * ("TEXT" "HTML" ("CHARSET" "UTF-8") NIL NIL "7BIT" 211 1 NIL NIL NIL) "ALTERNATIVE" ("BOUNDARY" "94eb2c11a5321c17bf0551d6864d") NIL NIL))"
 *
 * Where the "("TEXT" "PLAIN" ("CHARSET" "UTF-8") NIL NIL "7BIT" 143 6 NIL NIL NIL)" section describes one
 * mail part, and it can repeat. The below regex matches one of such a snippet, in a naive way.
 *
 * Meaning of the structure (elements, in order):
 * - body type
 * - body subtype
 * - parameter list (in parenthesis, list of key-value pairs)
 * - body id
 * - body description
 * - body encoding
 * - body size in bytes
 * - body size in lines
 * - body MD5
 * - body disposition
 * - body language
 * - body location
 *
 */
#define BODYPART_REGEX  "\\(\\\"[^\\\"]+\\\" \\\"[^\\\"]+\\\" \\([^\\)]+\\) [^\\s]+ [^\\s]+ [^\\s]+ [0-9]+ [0-9]+ [^\\s\\)]+ [^\\s\\)]+ [^\\s\\)]+\\)"

std::vector<std::string> getBodyElementStrings(const std::string& structure){
    std::vector<std::string> res;
    std::regex bodypart_regex(BODYPART_REGEX);
    std::smatch matches;

    std::string::const_iterator it = structure.begin();

    while (std::regex_search(it, structure.cend(), matches, bodypart_regex)){
        res.push_back(matches[0].str());
        it = matches.suffix().first;
    }

    return res;
}

std::string extractSubType(std::string structure){
    size_t start = 0;
    for (size_t i = 0; i < 3; ++i)
        start = structure.find('"', start + 1);
    size_t end = structure.find('"', start + 1);
    return structure.substr(start + 1, end - start - 1);
}

std::string extractType(std::string structure){
    size_t start = structure.find('"');
    size_t end = structure.find('"', start + 1);
    return structure.substr(start + 1, end - start - 1);
}

bool extractIsAttachment(std::string structure){
    return !(structure.find("\"NAME\"") == std::string::npos);
}

std::string extractId(std::string structure){
    size_t endOfParamList = structure.find(')');
    size_t start = endOfParamList + 2;
    size_t end = structure.find(' ', start + 1);

    std::string ret = structure.substr(start, end - start);

    // if it's NIL, return an empty string. Otherwise remove the quotes.
    if (ret == "NIL")
        ret = "";
    else
        ret = ret.substr(1, ret.length() - 2);

    return ret;
}

PartType getPartType(std::string partTypeString){
    DBG("Trying to parse {} into PartType", partTypeString);
    if (partTypeString == "TEXT")
        return PartType::TEXT;
    return PartType::NA;
}

PartSubType getPartSubType(std::string partSubTypeString){
    DBG("Trying to parse {} into PartSubType", partSubTypeString);
    if (partSubTypeString == "PLAIN")
        return PartSubType::PLAIN;
    if (partSubTypeString == "HTML")
        return PartSubType::HTML;
    return PartSubType::NA;
}

MailPart parseStructure(std::string structure){
    std::string type = extractType(structure);
    std::string subType = extractSubType(structure);
    bool isAttachment = extractIsAttachment(structure);
    std::string id = extractId(structure);
    MailPart mailPart;

    mailPart.partType = getPartType(type);
    mailPart.partSubType = getPartSubType(subType);
    mailPart.isAttachment = isAttachment;
    mailPart.id = id;
    return mailPart;
}

MailStructure::MailStructure(std::string structure)
{
    std::vector<std::string> bodyElementStrings = getBodyElementStrings(structure);

    for (const auto s: bodyElementStrings){
        sections.push_back(parseStructure(s));
    }

}

std::vector<MailPart> MailStructure::getAllSections()
{
    return sections;
}
