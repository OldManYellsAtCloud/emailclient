#include "parsers/imap/bodystructure.h"
#include "utils.h"
#include <loglibrary.h>

BodyStructure::BodyStructure(std::string s)
{
    success = false;

    size_t startOfParamList = s.find('(');
    size_t endOfParamList = s.find(')', startOfParamList + 1);
    std::vector<std::string> firstTwoParams = splitString(s.substr(0, startOfParamList - 1));

    if (firstTwoParams.size() != 2)
        return;

    std::string temp;
    std::istringstream(firstTwoParams.at(0)) >> std::quoted(temp);
    type = temp == "NIL" ? "" : temp;

    std::istringstream(firstTwoParams.at(1)) >> std::quoted(temp);
    subType = temp == "NIL" ? "" : temp;

    std::vector<std::string> paramList = splitString(s.substr(startOfParamList + 1, endOfParamList - startOfParamList + 1));

    std::string key, val;
    for (auto i = 0; i < paramList.size() && paramList.size() > 1; i += 2){
        std::istringstream(paramList.at(i)) >> std::quoted(key);
        std::istringstream(paramList.at(i + 1)) >> std::quoted(val);
        parameters[key] = val;
    }

    std::vector<std::string> lastParams = splitString(s.substr(endOfParamList + 2, s.length() - endOfParamList + 2));
    if (lastParams.size() != 8)
        return;

    std::istringstream(lastParams.at(0)) >> std::quoted(temp);
    id = temp == "NIL" ? "" : temp;

    std::istringstream(lastParams.at(1)) >> std::quoted(temp);
    description = temp == "NIL" ? "" : temp;

    std::istringstream(lastParams.at(2)) >> std::quoted(temp);
    encoding = temp == "NIL" ? "" : temp;

    std::istringstream(lastParams.at(3)) >> std::quoted(temp);
    try {
        size = temp == "NIL" ? 0 : stoi(temp);
    } catch (std::exception e) {
        ERROR("Could not parse size from body structure: {}", s);
        return;
    }

    std::istringstream(lastParams.at(4)) >> std::quoted(temp);
    md5 = temp == "NIL" ? "" : temp;

    std::istringstream(lastParams.at(5)) >> std::quoted(temp);
    disposition = temp == "NIL" ? "" : temp;

    std::istringstream(lastParams.at(6)) >> std::quoted(temp);
    language = temp == "NIL" ? "" : temp;

    std::istringstream(lastParams.at(7)) >> std::quoted(temp);
    location = temp == "NIL" ? "" : temp;

    success = true;
}
