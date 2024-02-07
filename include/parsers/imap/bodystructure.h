#ifndef BODYSTRUCTURE_H
#define BODYSTRUCTURE_H

#include <string>
#include <vector>
#include <map>

class BodyStructure
{
private:
    bool success;
    std::string type;
    std::string subType;
    std::map<std::string, std::string> parameters;
    std::string id;
    std::string description;
    std::string encoding;
    size_t size;
    std::string md5;
    std::string disposition;
    std::string language;
    std::string location;

public:
    explicit BodyStructure(std::string s);
    bool isSuccess(){return success;}
    std::string getType(){return type;}
    std::string getSubType(){return subType;}
    std::map<std::string, std::string> getParameters(){return parameters;}
    std::string getId(){return id;}
    std::string getDescription(){return description;}
    std::string getEncoding(){return encoding;}
    size_t getSize(){return size;}
    std::string getMd5(){return md5;}
    std::string getDisposition(){return disposition;}
    std::string getLanguage(){return language;}
    std::string getLocation(){return location;}
};

#endif // BODYSTRUCTURE_H
