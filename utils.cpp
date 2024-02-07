#include "utils.h"

#include <regex>
#include <QString>
#include <QByteArray>

bool isWhiteSpace(char c){
    return std::isspace((unsigned char)c);
}

std::string rtrim(std::string s){
    if (!s.empty() && isWhiteSpace(s[s.length() - 1]))
        return rtrim(s.substr(0, s.length() - 1));
    return s;
}

std::string ltrim(std::string s){
    if (!s.empty() && isWhiteSpace(s[0]))
        return ltrim(s.substr(1));
    return s;
}

std::string trim(std::string s){
    return ltrim(rtrim(s));
}

std::string constructImapUrl(std::string host, int port){
    if (port == IMAP_DEFAULT_PORT){
        return std::format("{}{}", IMAP_PROTOCOL_PREFIX, host);
    } else if (port == IMAPS_DEFAULT_PORT){
        return std::format("{}{}", IMAPS_PROTOCOL_PREFIX, host);
    }
    return std::format("{}{}:{}", IMAP_PROTOCOL_PREFIX, host, port);
}

bool isError(CURLcode err, std::string msg){
    if (err != CURLE_OK){
        ERROR("Curl error: {} - {}", int(err), curl_easy_strerror(err));
        ERROR("Extra details: {}", msg);
        return true;
    }
    return false;
}

std::string extractDateWithoutTimeZone(const std::string& date){
    size_t lastSpace = date.find_last_of(' ');
    size_t secondToLastSpace = date.find_last_of(' ', lastSpace - 1);
    return date.substr(0, secondToLastSpace);
}

std::string extractTimeZone(const std::string& date){
    size_t lastSpace = date.find_last_of(' ');
    size_t secondToLastSpace = date.find_last_of(' ', lastSpace - 1);
    return date.substr(secondToLastSpace + 1, lastSpace - secondToLastSpace - 1);;
}

std::tm adjustTimeWithTimeZone(std::tm t, std::string timeZone){
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

uint32_t dateStringToInt(std::string& date){
    std::tm t{};
    std::string timeZone = extractTimeZone(date);
    std::string dateWithoutTimeZone = extractDateWithoutTimeZone(date);

    std::stringstream ss{dateWithoutTimeZone};
    ss >> std::get_time(&t, "Date: %a, %d %b %Y %T");

    t = adjustTimeWithTimeZone(t, timeZone);
    auto timeSinceEpoch = mktime(&t);

    return timeSinceEpoch;
}

// One line looks like this:
// "blah blah" .. .. "optional quotes all around" " " "FINAL_FOLDER_NAME_IN_LAST_2_QUOTES"
// This extract the final name;
std::string parseFolderName(std::string line){
    std::string ret;
    size_t start = line.rfind('"', line.length() - 3);
    ret = line.substr(start, line.length() - 1 - start);
    // remove the enclosing quotes
    ret = ret.substr(1, ret.length() - 2);
    return ret;
}

std::vector<int> parseSearchResponseUids(std::string searchResponse){
    std::vector<int> ret;

    // empty response
    if (searchResponse == "* SEARCH\r\n")
        return ret;

    int lastSpace = searchResponse.length() - 2;
    int secondToLastSpace;
    std::string uid;
    while (lastSpace > 8){
        secondToLastSpace = searchResponse.find_last_of(' ', lastSpace - 1);
        uid = searchResponse.substr(secondToLastSpace + 1, lastSpace - secondToLastSpace - 1);
        ret.push_back(stoi(uid));
        lastSpace = secondToLastSpace;
    }
    return ret;
}

bool iCompareString(const std::string &s1, const std::string &s2)
{
    if (s1.length() != s2.length())
        return false;

    auto charCompareInsensitive = [](char c1, char c2) ->bool {return std::tolower(c1) == std::tolower(c2);};
    return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(), charCompareInsensitive);
}

std::vector<std::string> splitString(std::string s)
{
    std::vector<std::string> ret;
    std::string word;
    size_t idx = 0;


    while ((idx = s.find(' ')) != std::string::npos){
        word = s.substr(0, idx);
        ret.push_back(word);
        s.erase(0, idx + 1);
    }

    if (!s.empty())
        ret.push_back(s);

    return ret;
}

std::string decodeBase64Sections(std::string s)
{
    std::string ret = s;
    const std::string BASE64_REGEX = R"-(=\?UTF-8\?B\?([^\?]+)\?)-";
    const std::string BASE64_REGEX_WITH_PREFIX = R"-((=\?UTF-8\?B\?[^\?]+\?=*))-";

    std::regex r(BASE64_REGEX);
    std::regex rFullGroup(BASE64_REGEX_WITH_PREFIX);

    std::smatch match;
    std::smatch matchWithPrefix;
    // regex_replace replaces everything in 1 go, so it can't be used with match grousp that are completely different
    // so do it one-by-one, and use regex_replace with exact matches
    while (std::regex_search(s, match, r)){
        std::regex_search(s, matchWithPrefix, rFullGroup);

        std::string fullMatchWithPrefix = matchWithPrefix[1];
        std::string base64ToDecode = match[1];
        std::string base64Decoded = QByteArray::fromBase64(base64ToDecode.c_str()).toStdString();

        s = match.suffix();
        //ret = std::regex_replace(ret, std::regex(fullMatchWithPrefix), base64Decoded);
        size_t idx = ret.find(fullMatchWithPrefix);
        ret.replace(idx, fullMatchWithPrefix.size(), base64Decoded);
    }
    return ret;
}
