#ifndef IMAPREQUESTS_H
#define IMAPREQUESTS_H

#include <string>
#include <chrono>

#include <stdint.h>
#include <curl/curl.h>

#include "curlresponse.h"
#include "mailsettings.h"

#include "parsers/imap/examine.h"
#include "parsers/imap/capability.h"

#define IMAPS_PORT 993

class ImapRequest {
private:
    std::string serverAddress;

    int imapRequestDelayMs = 0;
    std::chrono::time_point<std::chrono::steady_clock> lastRequestTime;

    CURL *curl;
    curlResponse body, header;
    std::unique_ptr<MailSettings> mailSettings;

    void initializeCurl();
    CURLcode performCurlRequest();
    void prepareCurlRequest(const std::string& url, const std::string& customRequest = "");

    void waitForRateLimit();
    void configureImap();


public:
    ImapRequest();
    ~ImapRequest() = default;

    bool NOOP();
    Capability CAPABILITY();
    bool ENABLE(std::string capability);
    Examine EXAMINE(std::string folder);
    std::string LIST(std::string reference, std::string mailbox);
    std::string FETCH(std::string folder, uint32_t messageindex, std::string item);
    std::string FETCH_MULTI_MESSAGE(std::string folder, std::string indexRange, std::string item);
    std::string UID_FETCH(std::string folder, uint32_t uid, std::string item);

    std::string getHeaderValue();
    std::string getBodyValue();
};

#endif // IMAPREQUESTS_H
