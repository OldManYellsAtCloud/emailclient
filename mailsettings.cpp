#include "mailsettings.h"
#include <loglibrary.h>

#define DEFAULT_IMAP_PORT  993
#define DEFAULT_MAIL_NR_TO_FETCH  10

MailSettings::MailSettings(): settings{"/etc"}
{
}

QString MailSettings::getImapServerAddress()
{
    return QString::fromStdString(settings.getValue("mail", "imapServerAddress"));
}

int MailSettings::getImapServerPort()
{
    try {
        return std::stoi(settings.getValue("mail", "imapServerPort"));
    } catch (std::exception e) {
        ERROR("Could not get iamp server port: {}", e.what());
        return DEFAULT_IMAP_PORT;
    }
}

QString MailSettings::getUserName()
{
    return QString::fromStdString(settings.getValue("mail", "userName"));
}

QString MailSettings::getPassword()
{
    auto passwordBase64 = settings.getValue("mail", "password");
    auto password = QByteArray::fromBase64(QByteArray::fromStdString((passwordBase64)));
    return password;
}

QString MailSettings::getDbPath()
{
    return QString::fromStdString(settings.getValue("mail", "dbPath"));
}

int MailSettings::getMailNumberToFetch()
{
    try {
        return std::stoi(settings.getValue("mail", "mailNumberToFetch"));
    } catch (std::exception e) {
        ERROR("Could not get number of mails to fetch: {}", e.what());
        return DEFAULT_MAIL_NR_TO_FETCH;
    }
}

QStringList MailSettings::getWatchedFolders()
{

    QString folderString = QString::fromStdString(settings.getValue("mail", "watchedFolders"));
    QStringList folders = folderString.split(',');
    return folders;
}

