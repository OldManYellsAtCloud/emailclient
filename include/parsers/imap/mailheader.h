#ifndef MAILHEADER_H
#define MAILHEADER_H

#include <string>
#include <vector>
#include <stdint.h>
#include <ctime>

#define SUBJECT_REGEX R"-(Subject: ([^\r\n]*))-"
#define FROM_REGEX R"-(From:\s([^\r\n]*))-"
#define TO_REGEX R"-(To: ([^\r\n]*))-"
#define DATE_REGEX R"-(Date: ([^\r\n]*))-"

class MailHeader
{
    bool success;
    std::string from;
    std::vector<std::string> to;
    std::string subject;
    uint32_t date;

    /*uint32_t dateStringToInt(std::string s);
    std::tm adjustTimeWithTimeZone(std::tm t, std::string timeZone);
    std::string extractDateWithoutTimeZone(const std::string& date);
    std::string extractTimeZone(const std::string& date);*/
public:
    explicit MailHeader(std::string s);
    bool isSuccess(){return success;}
    std::string getFrom(){return from;}
    std::vector<std::string> getTo(){return to;}
    std::string getSubject(){return subject;}
    uint32_t getDate(){return date;}
};

#endif // MAILHEADER_H
