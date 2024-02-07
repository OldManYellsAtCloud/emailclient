#ifndef DBENGINE_H
#define DBENGINE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <vector>
#include <memory>
#include "mail.h"

class DbEngine
{
    QSqlDatabase db;
    std::unique_ptr<QSqlQuery> insertQuery;
    std::unique_ptr<QSqlQuery> retrieveQuery;
    std::unique_ptr<QSqlQuery> countMailQuery;
    std::unique_ptr<QSqlQuery> countAllMailQuery;
    std::unique_ptr<QSqlQuery> countMailInFolderQuery;
    std::unique_ptr<QSqlQuery> retrieveUidsFromFolderQuery;
    std::unique_ptr<QSqlQuery> countUnreadMailsInFolderQuery;
    std::unique_ptr<QSqlQuery> updateReadStatusQuery;

    const std::string CREATE_MAIL_TABLE = "CREATE TABLE IF NOT EXISTS mails (id INTEGER PRIMARY KEY AUTOINCREMENT, \
                                                                        uid INTEGER, \
                                                                        folder TEXT, \
                                                                        sender TEXT, \
                                                                        subject TEXT, \
                                                                        date INTEGER, \
                                                                        body TEXT, \
                                                                        read BOOLEAN)";
    const std::string CREATE_ATTACHMENT_TABLE = "CREATE TABLE IF NOT EXISTS attachments (id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                "mail_id INTEGER, attachment_name TEXT, attachment_content TEXT, FOREIGN KEY(mail_id) REFERENCES mails(id))";
    const std::string CREATE_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS mails_idx ON mails (uid, folder)";

    const std::string RETRIEVE_QUERY = "SELECT uid, folder, sender, subject, date, body, read FROM mails WHERE uid = :uid AND folder = :folder";
    const std::string INSERT_QUERY = "INSERT INTO mails(uid, folder, sender, subject, date, body, read) VALUES(:uid, :folder, :sender, :subject, :date, :body, :read)";
    const std::string COUNT_MAIL = "SELECT COUNT(*) AS c FROM mails WHERE uid = :uid AND folder = :folder";
    const std::string COUNT_ALL_MAIL = "SELECT COUNT(*) as c FROM mails";
    const std::string COUNT_MAIL_IN_FOLDER = "SELECT COUNT(*) as c FROM mails WHERE folder = :folder";
    const std::string RETRIEVE_UIDS_FROM_FOLDER = "SELECT uid FROM mails WHERE folder = :folder";
    const std::string COUNT_UNREAD_MAILS_IN_FOLDER = "SELECT COUNT(*) FROM mails WHERE read = 0 and folder = :folder";
    const std::string SET_READ_FLAG = "UPDATE mails SET read = :read WHERE folder = :folder AND uid = :uid";

    void createTablesIfNeeded();
    void prepareStatements();

public:
    explicit DbEngine(const std::string& path);
    mail retrieveEmail(const std::string& folder, const int& uid);
    std::vector<int> retreiveUidsFromFolder(const std::string& folder);
    bool storeEmail(mail& mail);
    bool isMailStored(const std::string&folder, const int&uid);
    bool isDbEmpty();
    int countMailsInFolder(std::string folder);
    int countUnreadMailsInFolder(std::string folder);
    bool updateReadStatus(std::string folder, int uid, bool read);

signals:

};

#endif // DBENGINE_H
