#ifndef MAILENGINE_H
#define MAILENGINE_H

#include <curl/curl.h>
#include "parsers/imap/examine.h"
#include "parsers/imap/folderlist.h"
#include "parsers/imap/capability.h"
#include "parsers/imap/mailheader.h"
#include "parsers/imap/bodystructures.h"
#include "parsers/imap/flags.h"
#include "parsers/imap/uid.h"

#include "curlresponse.h"
#include "mailsettings.h"

#include "dbengine.h"

#include "mail.h"

#include <QFile>

class MailEngine : public QObject
{
    Q_OBJECT
private:
    std::unique_ptr<DbEngine> dbEngine;
    CURL *curl;
    curlResponse body, header;
    std::unique_ptr<MailSettings> mailSettings;
    std::string serverAddress;
    QFile *unreadEmailFlag;

    void initializeCurl();
    void configureImap();
    void prepareCurlRequest(const std::string& url, const std::string& customRequest = "");

    int getBodyStructureIndex(BodyStructures& bs, const std::string& type, const std::string& subType);
    std::string parseBodyContentOnly(const std::string& bodyResponse);

    CURLcode performCurlRequest();
public:
    MailEngine();
    ~MailEngine();
    bool NOOP();
    Capability CAPABILITY();
    bool ENABLE(std::string capability);
    Examine EXAMINE(std::string folder);
    std::string LIST(std::string reference, std::string mailbox);
    std::string FETCH(std::string folder, uint32_t messageindex, std::string item);
    std::string FETCH_MULTI_MESSAGE(std::string folder, std::string indexRange, std::string item);
    std::string UID_FETCH(std::string folder, uint32_t uid, std::string item);

    FolderList folderList();
    Uid getMessageUid(std::string folder, uint32_t messageindex);
    MailHeader getMessageHeaders(std::string folder, uint32_t messageindex);
    BodyStructures getBodyStructure(std::string folder, uint32_t messageindex);
    std::string getTextBody(std::string folder, uint32_t messageindex);
    Flags getMessageFlags(std::string folder, uint32_t messageindex);
    std::map<uint32_t, Flags> getMessageFlagsForMultipleMails(std::string folder, uint32_t startIndex, uint32_t endIndex);

    mail getMail(std::string folder, uint32_t messageindex);

    int fetchAllMails(std::string folder);
    void doInitialLoad();

    int countCachedMails(std::string folder);
    std::vector<mail> fetchCachedMailsFromFolder(std::string folder);
    int fetchInitialMails(std::string folder);
    int fetchNewMails(std::string folder);

    void fetchMailsFromWatchedFolders();
    bool setUnreadFlagForWatchedFolders();

    void updateCachedReadFlag(std::string folder);

signals:
    void newMailFetched(std::string folder);
};

#endif // MAILENGINE_H
