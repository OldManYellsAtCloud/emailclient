#include "imaprequests.h"
#include <loglibrary.h>
#include <thread>

#include <QUrl> // for percent encoding

std::string constructUrl(std::string host, int port){
    auto prefix = port == IMAPS_PORT ? "imaps://" : "imap://";
    return std::format("{}{}:{}", prefix, host, port);
}

void ImapRequest::waitForRateLimit()
{
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    auto diffSinceLastRequest = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRequestTime);

    if (diffSinceLastRequest.count() < imapRequestDelayMs) {
        auto sleepBeforeNextRequest = imapRequestDelayMs - diffSinceLastRequest.count();
        DBG("Rate limit, sleep {}ms", sleepBeforeNextRequest);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepBeforeNextRequest));
    }
}

ImapRequest::ImapRequest()
{
    mailSettings = std::make_unique<MailSettings>();
    imapRequestDelayMs = mailSettings->getImapRequestDelay();
    lastRequestTime = std::chrono::steady_clock::now();
    initializeCurl();
}

static size_t storeCurlData(char *ptr, size_t size, size_t nmemb, void *userdata){
    curlResponse *cr = (curlResponse*)userdata;
    std::string s(ptr, nmemb);
    DBG("Store data called with fragment: {}", ptr);
    return cr->storeResponse(ptr, nmemb);
}

void ImapRequest::prepareCurlRequest(const std::string& url, const std::string& customRequest)
{
    header.reset();
    body.reset();
    LOG("Preparing request with url: {}, command: {}", url, customRequest);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeCurlData);

    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, storeCurlData);

    if (customRequest == "")
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
    else
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, customRequest.c_str());
}

void ImapRequest::initializeCurl()
{
    curl = curl_easy_init();

    auto imapAddress = mailSettings->getImapServerAddress().toStdString();
    int imapPort = mailSettings->getImapServerPort();

    serverAddress = constructUrl(imapAddress, imapPort);

    auto userName = mailSettings->getUserName().toStdString();
    auto password = mailSettings->getPassword().toStdString();

    curl_easy_setopt(curl, CURLOPT_USERNAME, userName.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
}

CURLcode ImapRequest::performCurlRequest()
{

    waitForRateLimit();
    lastRequestTime = std::chrono::steady_clock::now();

    auto res = curl_easy_perform(curl);

    DBG("Body response: {}", getBodyValue().substr(0, 1000));
    DBG("Header response: {}", getHeaderValue().substr(0, 1000));

    return res;
}

bool ImapRequest::NOOP()
{
    prepareCurlRequest(serverAddress, "NOOP");
    performCurlRequest();
    return body.success();
}

Capability ImapRequest::CAPABILITY()
{
    prepareCurlRequest(serverAddress, "CAPABILITY");
    performCurlRequest();
    Capability ret (getBodyValue());

    return ret;
}

bool ImapRequest::ENABLE(std::string capability)
{
    std::string cmd = std::format("ENABLE {}", capability);
    prepareCurlRequest(serverAddress, cmd);
    performCurlRequest();
    return body.success();
}

Examine ImapRequest::EXAMINE(std::string folder)
{
    folder = QUrl::toPercentEncoding(QString::fromStdString(folder)).toStdString();
    std::string cmd = std::format("EXAMINE {}", folder);
    prepareCurlRequest(serverAddress, cmd);
    performCurlRequest();
    Examine ret (getBodyValue());
    return ret;
}

std::string ImapRequest::LIST(std::string reference, std::string mailbox)
{
    mailbox = QUrl::toPercentEncoding(QString::fromStdString(mailbox)).toStdString();
    std::string cmd = std::format("LIST {} {}", reference, mailbox);
    prepareCurlRequest(serverAddress, cmd);
    performCurlRequest();
    return getBodyValue();
}

std::string ImapRequest::FETCH(std::string folder, uint32_t messageindex, std::string item)
{
    folder = QUrl::toPercentEncoding(QString::fromStdString(folder)).toStdString();
    std::string addr = std::format("{}/{}", serverAddress, folder);
    std::string cmd = std::format("FETCH {} {}", messageindex, item);
    prepareCurlRequest(addr, cmd);
    performCurlRequest();
    return getBodyValue();
}

std::string ImapRequest::FETCH_MULTI_MESSAGE(std::string folder, std::string indexRange, std::string item)
{
    folder = QUrl::toPercentEncoding(QString::fromStdString(folder)).toStdString();
    std::string addr = std::format("{}/{}", serverAddress, folder);
    std::string cmd = std::format("FETCH {} {}", indexRange, item);
    prepareCurlRequest(addr, cmd);
    performCurlRequest();
    return getBodyValue();
}

std::string ImapRequest::UID_FETCH(std::string folder, uint32_t uid, std::string item)
{
    folder = QUrl::toPercentEncoding(QString::fromStdString(folder)).toStdString();
    std::string addr = std::format("{}/{}", serverAddress, folder);
    std::string cmd = std::format("UID FETCH {} {}", uid, item);
    prepareCurlRequest(addr, cmd);
    performCurlRequest();
    return getBodyValue();
}

std::string ImapRequest::getHeaderValue()
{
    return header.getResponse();
}

std::string ImapRequest::getBodyValue()
{
    return body.getResponse();
}