#ifndef EXAMINE_H
#define EXAMINE_H

#include <vector>
#include <string>

#define FLAGS_REGEX R"-([^T]FLAGS \(([^\)]*))-"
#define PERMANENT_FLAGS_REGEX R"-(TFLAGS \(([^\)]*))-"
#define UIDVALIDITY_REGEX "UIDVALIDITY ([0-9]+)"
#define UIDNEXT_REGEX "UIDNEXT ([0-9]+)"
#define EXISTS_REGEX "([0-9]+) EXISTS"
#define RECENT_REGEX "([0-9]+) RECENT"

class Examine
{
private:
    bool success;
    std::vector<std::string> flags;
    std::vector<std::string> permanentFlags;
    int exists;
    int recent;
    int uidNext;
    int uidValidity;

    int parseInt(std::string s);

public:
    explicit Examine(std::string s);
    ~Examine(){}
    bool isSuccess(){return success;}
    std::vector<std::string> getFlags(){return flags;}
    std::vector<std::string> getPermanentFlags(){return permanentFlags;}
    int getExists(){return exists;}
    int getRecent(){return recent;}
    int getUidNext(){return uidNext;}
    int getUidValidity(){return uidValidity;}
};

#endif // EXAMINE_H
