#include "parsers/imap/folderlist.h"
#include <loglibrary.h>
#include <regex>

FolderList::FolderList(std::string s)
{
    std::regex folderRegex(FOLDER_REGEX);
    std::smatch match;
    while (std::regex_search(s, match, folderRegex)){
        folderList.push_back(match[1]);
        LOG("Found folder: {}", match[1].str());
        s = match.suffix().str();
    }
    success = true;
}
