#include "mailengine.h"
#include "loglibrary.h"
#include "utils.h"

#include <QUrl> // for percent encoding
#include <filesystem> // for temp folder
#include <regex>


MailEngine::MailEngine() {
    dbEngine = std::make_unique<DbEngine>(mailSettings->getDbPath().toStdString());
    imapRequest = std::make_unique<ImapRequest>();

    std::string unreadEmailFlagPath = std::format("{}/{}", std::filesystem::temp_directory_path().string(), "unreadMailFlag");
    unreadEmailFlag = new QFile(QString::fromStdString(unreadEmailFlagPath));
}

MailEngine::~MailEngine()
{
    delete unreadEmailFlag;
}


int MailEngine::getBodyStructureIndex(BodyStructures&  bs, const std::string& type, const std::string& subType)
{
    int ret = -1;

    for (int i = 0; i < bs.getBodyStructures().size(); ++i){
        auto bodyStruct = bs.getBodyStructures().at(i);

        if (iCompareString(type, bodyStruct.getType())) {
            // TODO: make half-type configurable?
            ret = i; // it's at least a half match, which is better than nothing
            if (iCompareString(subType, bodyStruct.getSubType())) // we got a full match
                return ret;
        }
    }

    return ret;
}

// remove the first line and last lines, also the last ")" at the end - these are
// part of the imap response, but not part of the mail itself
std::string MailEngine::parseBodyContentOnly(const std::string &bodyResponse)
{
    std::string newline = "\r\n";

    std::string ret;
    if (bodyResponse.size() == 0)
        return ret;

    size_t endOfFirstLine = bodyResponse.find(newline);
    ret = bodyResponse.substr(endOfFirstLine + newline.length(), bodyResponse.length() - endOfFirstLine);

    size_t startOfLastLine = ret.rfind(newline, ret.length() - newline.length() - 1);
    ret = ret.substr(0, startOfLastLine - 1);
    return ret;
}


FolderList MailEngine::folderList()
{
    std::string listResponse = imapRequest->LIST("\"\"", "*");
    FolderList ret(listResponse);
    return ret;
}

Uid MailEngine::getMessageUid(std::string folder, uint32_t messageindex)
{
    std::string response = imapRequest->FETCH(folder, messageindex, "UID");

    Uid ret(response);
    return ret;
}


MailHeader MailEngine::getMessageHeaders(std::string folder, uint32_t messageindex)
{
    std::string response = imapRequest->FETCH(folder, messageindex, "BODY.PEEK[HEADER]");
    MailHeader ret(imapRequest->getHeaderValue());
    return ret;
}

BodyStructures MailEngine::getBodyStructure(std::string folder, uint32_t messageindex)
{
    imapRequest->FETCH(folder, messageindex, "BODYSTRUCTURE");
    std::string bodyStructureResponse = imapRequest->getBodyValue();

    BodyStructures ret(bodyStructureResponse);
    return ret;

}

std::string MailEngine::getTextBody(std::string folder, uint32_t messageindex)
{
    BodyStructures bodyStructure = getBodyStructure(folder, messageindex);
    // TODO: store separate HTML and TEXT? let's see if it will be ever needed...
    int textOrHtmlBodyIndex = getBodyStructureIndex(bodyStructure, "TEXT", "PLAIN") + 1; // IMAP uses 1-based indexing
    //std::string peekCommand = std::format("BODY.PEEK[{}]", textOrHtmlBodyIndex);
    std::string peekCommand = "BODY.PEEK[TEXT]";

    imapRequest->FETCH(folder, messageindex, peekCommand);
    std::string textBodyResponse = imapRequest->getHeaderValue();
    textBodyResponse = parseBodyContentOnly(textBodyResponse);

    return textBodyResponse;
}

Flags MailEngine::getMessageFlags(std::string folder, uint32_t messageIndex)
{
    imapRequest->FETCH(folder, messageIndex, "FLAGS");
    std::string flagsResponse = imapRequest->getHeaderValue();
    Flags ret(flagsResponse);
    return ret;
}

std::map<uint32_t, Flags> MailEngine::getMessageFlagsForMultipleMails(std::string folder, uint32_t startIndex, uint32_t endIndex)
{
    std::string MULTI_MESSAGE_FLAGS_REGEX = R"-(([0-9]+) FETCH \(UID ([0-9]+) (FLAGS \([^)]*\)))-";
    std::regex r(MULTI_MESSAGE_FLAGS_REGEX);
    std::map<uint32_t, Flags> ret;

    std::string indices = std::format("{}:{}", startIndex, endIndex);
    imapRequest->FETCH_MULTI_MESSAGE(folder, indices, "(FLAGS UID)");

    std::string response = imapRequest->getHeaderValue();
    std::smatch matches;
    while (std::regex_search(response, matches, r)){
        try {
            uint32_t uid = stoi(matches[2]);
            std::string flagsString = matches[3];
            ret.insert(std::make_pair(uid, Flags(flagsString)));
        } catch (std::exception e){
            ERROR("Could not parse message uid from string: {}", response);
            break;
        }

        response = matches.suffix();
    }

    return ret;
}

mail MailEngine::getMail(std::string folder, uint32_t messageindex, bool ignoreCache)
{
    mail ret;
    Uid uid = getMessageUid(folder, messageindex);
    if (!ignoreCache && dbEngine->isMailStored(folder, uid.getUid())) {
        return dbEngine->retrieveEmail(folder, uid.getUid());
    } else {
        auto header = getMessageHeaders(folder, messageindex);
        auto body = getTextBody(folder, messageindex);
        auto flags = getMessageFlags(folder, messageindex).getFlags();

        bool read = std::find(flags.begin(), flags.end(), "\\Seen") != flags.end();

        ret.setBody(body);
        ret.setSubject(header.getSubject());
        ret.setDate(header.getDate());
        ret.setFrom(header.getFrom());
        ret.setUid(uid.getUid());
        ret.setFolder(folder);
        ret.setRead(read);

        if (!ignoreCache)
            dbEngine->storeEmail(ret);

        emit newMailFetched(folder);

        return ret;
    }
}

int MailEngine::fetchAllMails(std::string folder)
{
    int cachedMailsInFolder = dbEngine->countMailsInFolder(folder);
    bool wasFolderEverFetched = cachedMailsInFolder > 0;

    if (wasFolderEverFetched) {
        LOG("Folder {} is cached", folder);
        return fetchNewMails(folder);
    } else {
        LOG("Folder {} is not cached", folder);
        return fetchInitialMails(folder);
    }
}

void MailEngine::doInitialLoad()
{
    if (dbEngine->isDbEmpty()){
        auto fl = folderList().getFolderList();
        for (const auto& folder: fl){
            fetchAllMails(folder);
        }
    }
}

int MailEngine::countCachedMails(std::string folder)
{
    return dbEngine->countMailsInFolder(folder);
}

std::vector<mail> MailEngine::fetchCachedMailsFromFolder(std::string folder)
{
    std::vector<int> uids = dbEngine->retreiveUidsFromFolder(folder);
    std::vector<mail> ret;
    for (const int& uid: uids){
        mail m = dbEngine->retrieveEmail(folder, uid);
        ret.push_back(m);
    }

    auto emailSorter = [](mail& a, mail& b)->bool{return a.getDate() > b.getDate();};
    std::sort(ret.begin(), ret.end(), emailSorter);

    return ret;
}

int MailEngine::fetchInitialMails(std::string folder)
{
    int maxNumberOfMailsToFetch = mailSettings->getMailNumberToFetch();
    int numberOfMailsInFolder = imapRequest->EXAMINE(folder).getExists();

    if (numberOfMailsInFolder == 0)
        return 0;

    maxNumberOfMailsToFetch = numberOfMailsInFolder > maxNumberOfMailsToFetch ?
                                  maxNumberOfMailsToFetch : numberOfMailsInFolder;

    int startingIndex = numberOfMailsInFolder;

    while (startingIndex > (numberOfMailsInFolder - maxNumberOfMailsToFetch)){
        getMail(folder, startingIndex--);
    }
    return maxNumberOfMailsToFetch;
}

int MailEngine::fetchNewMails(std::string folder)
{
    int fetchedEmails = 0;
    std::vector<int> cachedUids = dbEngine->retreiveUidsFromFolder(folder);
    std::vector<int>::iterator lastCachedUid;
    if (cachedUids.size() == 0)
        cachedUids.push_back(0);

    int numberOfMailsInFolder = imapRequest->EXAMINE(folder).getExists();
    Uid lastOnlineUid = getMessageUid(folder, numberOfMailsInFolder);
    LOG("Last cached UID: {}, folder: {}", *lastCachedUid, folder);
    LOG("Last online UID: {}", lastOnlineUid.getUid());

    while (lastOnlineUid.getUid() != *lastCachedUid){

        getMail(folder, numberOfMailsInFolder--);
        lastOnlineUid = getMessageUid(folder, numberOfMailsInFolder);
        ++fetchedEmails;
    }
    return fetchedEmails;
}

void MailEngine::fetchMailsFromWatchedFolders()
{
    QStringList watchedFolders = mailSettings->getWatchedFolders();
    for (QString s: watchedFolders){
        fetchAllMails(s.toStdString());
        updateCachedReadFlag(s.toStdString());
    }
}

bool MailEngine::setUnreadFlagForWatchedFolders()
{
    QStringList watchedFolders = mailSettings->getWatchedFolders();
    for (QString s: watchedFolders){
        if (dbEngine->countUnreadMailsInFolder(s.toStdString()) > 0){
            if (!unreadEmailFlag->exists()){
                unreadEmailFlag->open(QIODevice::WriteOnly);
                unreadEmailFlag->close();
            }
            return true;
        }
    }

    if (unreadEmailFlag->exists()){
        unreadEmailFlag->remove();
    }

    return false;
}

void MailEngine::updateCachedReadFlag(std::string folder)
{
    int numberOfCachedMails = dbEngine->countMailsInFolder(folder);
    int numberOfMaxMailsToExamine = mailSettings->getMailNumberToFetch();

    if (numberOfCachedMails == 0 || numberOfMaxMailsToExamine == 0)
        return;

    if (numberOfCachedMails < numberOfMaxMailsToExamine)
        numberOfMaxMailsToExamine = numberOfCachedMails;

    int maxOnlineIndex = imapRequest->EXAMINE(folder).getExists();
    int firstMailIndex = maxOnlineIndex - numberOfMaxMailsToExamine;
    std::map<uint32_t, Flags> messageFlags = getMessageFlagsForMultipleMails(folder, firstMailIndex, maxOnlineIndex);
    for (auto it = messageFlags.begin(); it != messageFlags.end(); ++it){
        auto flags = it->second.getFlags();
        bool isMessageRead = std::find(flags.begin(), flags.end(), "\\Seen") != flags.end();
        dbEngine->updateReadStatus(folder, it->first, isMessageRead);
    }
}

