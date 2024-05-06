#include "parsers/imap/mailheader.h"
#include "utils.h"
#include <loglibrary.h>
#include <regex>
#include <iomanip>

/*
uint32_t MailHeader::dateStringToInt(std::string s)
{
    std::tm t{};
    std::string timeZone = extractTimeZone(s);
    std::string dateWithoutTimeZone = extractDateWithoutTimeZone(s);

    std::stringstream ss{dateWithoutTimeZone};
    ss >> std::get_time(&t, "%a, %d %b %Y %T");

    t = adjustTimeWithTimeZone(t, timeZone);
    auto timeSinceEpoch = mktime(&t);

    return timeSinceEpoch;
}

tm MailHeader::adjustTimeWithTimeZone(tm t, std::string timeZone)
{
    int timeZoneHours, timeZoneMinutes;
    try {
        timeZoneHours = stoi(timeZone.substr(0, 3));
        timeZoneMinutes = stoi(timeZone.substr(3, 2));
    } catch (std::exception e){
        ERROR("Could not extract timeZone from string: {}", timeZone);
        timeZoneHours = 0;
        timeZoneMinutes = 0;
    }

    t.tm_hour -= timeZoneHours;

    if (timeZoneHours < 0){
        t.tm_min += timeZoneMinutes;
    } else {
        t.tm_min -= timeZoneMinutes;
    }
    return t;
}

std::string MailHeader::extractDateWithoutTimeZone(const std::string &date)
{
    size_t lastSpace = date.find_last_of(' ');
    size_t secondToLastSpace = date.find_last_of(' ', lastSpace - 1);
    return date.substr(0, secondToLastSpace);
}

std::string MailHeader::extractTimeZone(const std::string &date)
{
    size_t lastSpace = date.find_last_of(' ');
    size_t secondToLastSpace = date.find_last_of(' ', lastSpace - 1);
    return date.substr(secondToLastSpace + 1, lastSpace - secondToLastSpace - 1);
}*/

MailHeader::MailHeader(std::string s)
{
    std::regex fromRegex(FROM_REGEX);
    std::regex toRegex(TO_REGEX);
    std::regex subjectRegex(SUBJECT_REGEX);
    std::regex dateRegex(DATE_REGEX);

    std::smatch match;

    if (std::regex_search(s.cbegin(), s.cend(), match, fromRegex))
        from = match[1];

    if (std::regex_search(s.cbegin(), s.cend(), match, subjectRegex))
        subject = match[1];

    if (std::regex_search(s.cbegin(), s.cend(), match, dateRegex)){
        LOG("Date parsed from mail header: {}", match[1].str());
        auto d = match[1].str();
        date = dateStringToInt(d);
    }

    if (std::regex_search(s.cbegin(), s.cend(), match, toRegex)){
        std::string extractedTo = match[1];
        if (extractedTo.find(',') == std::string::npos){
            to.push_back(extractedTo);
        } else {
            to = splitString(extractedTo);
        }
    }

    success = true;
}
