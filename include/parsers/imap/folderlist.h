#ifndef FOLDERLIST_H
#define FOLDERLIST_H

#include <vector>
#include <string>

// TODO: filter out folder names with "HasChildren" flag, as they are top level collection-folders, containing other folders
#define FOLDER_REGEX "\\\"([^\"]+)\\\"\r"

class FolderList
{
private:
    bool success;
    std::vector<std::string> folderList;
public:
    explicit FolderList(std::string s);
    bool isSuccess(){return success;}
    std::vector<std::string> getFolderList(){return folderList;}
};

#endif // FOLDERLIST_H
