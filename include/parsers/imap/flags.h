#ifndef FLAGS_H
#define FLAGS_H

#include <vector>
#include <string>
#include <map>

#define MESSAGE_FLAGS_REGEX R"-(FLAGS\s\(([^)]*)\))-"

class Flags
{
private:
    bool success;
    std::vector<std::string> flags;
public:
    explicit Flags(std::string s);
    bool isSuccess(){return success;}
    std::vector<std::string> getFlags(){return flags;}
};

#endif // FLAGS_H
