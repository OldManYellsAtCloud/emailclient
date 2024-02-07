#ifndef UID_H
#define UID_H

#include <string>

#define UID_REGEX R"-(UID\s([0-9]*))-"

class Uid
{
    bool success;
    int uid;
public:
    explicit Uid(std::string s);
    bool isSuccess(){return success;}
    int getUid(){return uid;}
};

#endif // UID_H
