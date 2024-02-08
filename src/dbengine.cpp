#include <loglibrary.h>
#include <QSqlRecord>
#include <QSqlError>

#include "dbengine.h"

void DbEngine::createTablesIfNeeded()
{
    QSqlQuery createTable;
    bool res = createTable.exec(QString::fromStdString(CREATE_MAIL_TABLE));

    if (!res){
        ERROR("Could not create mails table in database!");
        exit(1);
    }

    res = createTable.exec(QString::fromStdString(CREATE_INDEX));

    if (!res){
        ERROR("Could not create mails index in database!");
        exit(1);
    }

    res = createTable.exec(QString::fromStdString(CREATE_ATTACHMENT_TABLE));

    if (!res){
        ERROR("Could not create attachments table in database!");
        exit(1);
    }
}

void DbEngine::prepareStatements()
{
    insertQuery = std::make_unique<QSqlQuery>(db);
    insertQuery->prepare(QString::fromStdString(INSERT_QUERY));

    retrieveQuery = std::make_unique<QSqlQuery>(db);
    retrieveQuery->prepare(QString::fromStdString(RETRIEVE_QUERY));

    countMailQuery = std::make_unique<QSqlQuery>(db);
    countMailQuery->prepare(QString::fromStdString(COUNT_MAIL));

    countAllMailQuery = std::make_unique<QSqlQuery>(db);
    countAllMailQuery->prepare(QString::fromStdString(COUNT_ALL_MAIL));

    countMailInFolderQuery = std::make_unique<QSqlQuery>(db);
    countMailInFolderQuery->prepare(QString::fromStdString(COUNT_MAIL_IN_FOLDER));

    retrieveUidsFromFolderQuery = std::make_unique<QSqlQuery>(db);
    retrieveUidsFromFolderQuery->prepare(QString::fromStdString(RETRIEVE_UIDS_FROM_FOLDER));

    countUnreadMailsInFolderQuery = std::make_unique<QSqlQuery>(db);
    countUnreadMailsInFolderQuery->prepare(QString::fromStdString(COUNT_UNREAD_MAILS_IN_FOLDER));

    updateReadStatusQuery = std::make_unique<QSqlQuery>(db);
    updateReadStatusQuery->prepare(QString::fromStdString(SET_READ_FLAG));
}

DbEngine::DbEngine(const std::string& path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(path));
    if (!db.open()){
        ERROR("Could not open database: {}", path);
        exit(1);
    }

    createTablesIfNeeded();
    prepareStatements();
}

mail DbEngine::retrieveEmail(const std::string &folder, const int &uid)
{
    retrieveQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(folder));
    retrieveQuery->bindValue(QString::fromStdString(":uid"), uid);

    bool res = retrieveQuery->exec();
    if (!res){
        ERROR("Could not execute query: {}", retrieveQuery->lastQuery().toStdString());
    }

    retrieveQuery->first();

    mail mail{};

    int bodyNo = retrieveQuery->record().indexOf("body");
    int fromNo = retrieveQuery->record().indexOf("sender");
    int subjectNo = retrieveQuery->record().indexOf("subject");
    int readNo = retrieveQuery->record().indexOf("read");
    int dateNo = retrieveQuery->record().indexOf("date");

    mail.setUid(uid);
    mail.setFolder(folder);
    mail.setBody(retrieveQuery->value(bodyNo).toString().toStdString());
    mail.setSubject(retrieveQuery->value(subjectNo).toString().toStdString());
    mail.setFrom(retrieveQuery->value(fromNo).toString().toStdString());
    mail.setDate(retrieveQuery->value(dateNo).toInt());
    mail.setRead(retrieveQuery->value(readNo).toBool());

    return mail;
}

std::vector<int> DbEngine::retreiveUidsFromFolder(const std::string &folder)
{
    std::vector<int> uids;
    retrieveUidsFromFolderQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(folder));
    bool ret = retrieveUidsFromFolderQuery->exec();
    if (!ret){
        ERROR("Could not retrieve UIDs from the db: {}", retrieveUidsFromFolderQuery->lastQuery().toStdString());
        ERROR("Last error: {}", retrieveUidsFromFolderQuery->lastError().text().toStdString());
        return uids;
    }

    while (retrieveUidsFromFolderQuery->next()){
        int uid = retrieveUidsFromFolderQuery->value(0).toInt();
        uids.push_back(uid);
    }

    return uids;
}

bool DbEngine::storeEmail(mail& mail)
{
    insertQuery->bindValue(QString::fromStdString(":uid"), mail.getUid());
    insertQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(mail.getFolder()));
    insertQuery->bindValue(QString::fromStdString(":body"), QString::fromStdString(mail.getBody()));
    insertQuery->bindValue(QString::fromStdString(":subject"), QString::fromStdString(mail.getSubject()));
    insertQuery->bindValue(QString::fromStdString(":sender"), QString::fromStdString(mail.getFrom()));
    insertQuery->bindValue(QString::fromStdString(":read"), mail.isRead());
    insertQuery->bindValue(QString::fromStdString(":date"), mail.getDate());

    bool ret = insertQuery->exec();
    if (!ret){
        ERROR("Could not execute query: {}", insertQuery->lastQuery().toStdString());
        ERROR("Last error: {}", insertQuery->lastError().text().toStdString());
    }

    return ret;
}

bool DbEngine::isMailStored(const std::string &folder, const int &uid)
{
    countMailQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(folder));
    countMailQuery->bindValue(QString::fromStdString(":uid"), uid);

    bool res = countMailQuery->exec();
    if (!res){
        ERROR("Could not execute count mail query: {}", countMailQuery->lastQuery().toStdString());
        return false;
    }
    countMailQuery->first();

    LOG("CountMailQuery result: {}", countMailQuery->value(0).toInt());
    bool ret = countMailQuery->value(countMailQuery->record().indexOf("c")).toInt() > 0;
    return ret;
}

bool DbEngine::isDbEmpty()
{
    bool res = countAllMailQuery->exec();
    if (!res){
        ERROR("Coiult not execute count all mail query: {}", countAllMailQuery->lastQuery().toStdString());
        return false;
    }

    countAllMailQuery->first();

    LOG("Number of locally stored mails: {}", countAllMailQuery->value(0).toInt());
    return countAllMailQuery->value(0).toInt() == 0;
}

int DbEngine::countMailsInFolder(std::string folder)
{
    countMailInFolderQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(folder));
    bool res = countMailInFolderQuery->exec();
    if (!res){
        ERROR("Could not count mails in folder: {}", countMailInFolderQuery->lastQuery().toStdString());
        return 0;
    }
    countMailInFolderQuery->first();
    LOG("Number of mails in {}: {}", folder, countMailInFolderQuery->value(0).toInt());
    return countMailInFolderQuery->value(0).toInt();
}

int DbEngine::countUnreadMailsInFolder(std::string folder)
{
    countUnreadMailsInFolderQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(folder));
    bool res = countUnreadMailsInFolderQuery->exec();

    if (!res) {
        ERROR("Could not count unread mails with query: {}", countUnreadMailsInFolderQuery->lastQuery().toStdString());
        ERROR("Last error: {}", countUnreadMailsInFolderQuery->lastError().text().toStdString());
        return 0;
    }

    countUnreadMailsInFolderQuery->first();
    return countUnreadMailsInFolderQuery->value(0).toInt();
}

bool DbEngine::updateReadStatus(std::string folder, int uid, bool read)
{
    updateReadStatusQuery->bindValue(QString::fromStdString(":folder"), QString::fromStdString(folder));
    updateReadStatusQuery->bindValue(QString::fromStdString(":uid"), uid);
    updateReadStatusQuery->bindValue(QString::fromStdString(":read"), read);

    bool res = updateReadStatusQuery->exec();
    if (!res) {
        ERROR("Could not set last 'read' status with query: {}", updateReadStatusQuery->lastQuery().toStdString());
        ERROR("Last error: {}", updateReadStatusQuery->lastError().text().toStdString());
    }
    return res;
}

