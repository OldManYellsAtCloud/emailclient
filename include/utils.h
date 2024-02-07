#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>
#include <loglibrary.h>
#include <cctype>

#include <stdint.h>
#include <iomanip>
#include <vector>

#define IMAP_DEFAULT_PORT 143
#define IMAPS_DEFAULT_PORT 993

#define IMAP_PROTOCOL_PREFIX  "imap://"
#define IMAPS_PROTOCOL_PREFIX "imaps://"


bool isWhiteSpace(char c);
std::string rtrim(std::string s);
std::string ltrim(std::string s);
std::string trim(std::string s);

std::string constructImapUrl(std::string host, int port);
bool isError(CURLcode err, std::string msg);
std::string extractDateWithoutTimeZone(const std::string& date);
std::string extractTimeZone(const std::string& date);
std::tm adjustTimeWithTimeZone(std::tm t, std::string timeZone);

uint32_t dateStringToInt(std::string& date);
std::string parseFolderName(std::string line);
std::vector<int> parseSearchResponseUids(std::string searchResponse);

bool iCompareString(const std::string& s1, const std::string& s2);

std::vector<std::string> splitString(std::string s);

std::string decodeBase64Sections(std::string s);

#endif // UTILS_H
