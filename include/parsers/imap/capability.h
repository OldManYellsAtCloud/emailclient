#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <vector>
#include <string>

#define CAPABILITY_STRING "CAPABILITY"


class Capability
{
    bool success;
    std::vector<std::string> capabilities;
public:
    explicit Capability(std::string s);
    bool isSuccess(){return success;}
    std::vector<std::string> getCapabilities(){return capabilities;}
};

#endif // CAPABILITY_H
