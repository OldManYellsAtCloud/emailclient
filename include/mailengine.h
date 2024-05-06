#ifndef MAILENGINE_H
#define MAILENGINE_H

#include "parsers/imap/folderlist.h"
#include "parsers/imap/mailheader.h"
#include "parsers/imap/bodystructures.h"
#include "parsers/imap/flags.h"
#include "parsers/imap/uid.h"

#include "mailsettings.h"

#include "dbengine.h"
#include "imaprequests.h"

#include "mail.h"

#include <QFile>

class MailEngine : public QObject
{
    Q_OBJECT
private:
    std::unique_ptr<DbEngine> dbEngine;

    std::unique_ptr<MailSettings> mailSettings;
    QFile *unreadEmailFlag;

    int getBodyStructureIndex(BodyStructures& bs, const std::string& type, const std::string& subType);
    std::string parseBodyContentOnly(const std::string& bodyResponse);
    std::unique_ptr<ImapRequest> imapRequest;

public:
    MailEngine();
    ~MailEngine();

    FolderList folderList();
    Uid getMessageUid(std::string folder, uint32_t messageindex);
    MailHeader getMessageHeaders(std::string folder, uint32_t messageindex);
    BodyStructures getBodyStructure(std::string folder, uint32_t messageindex);
    std::string getTextBody(std::string folder, uint32_t messageindex);
    Flags getMessageFlags(std::string folder, uint32_t messageindex);
    std::map<uint32_t, Flags> getMessageFlagsForMultipleMails(std::string folder, uint32_t startIndex, uint32_t endIndex);

    mail getMail(std::string folder, uint32_t messageindex, bool ignoreCache = false);

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
