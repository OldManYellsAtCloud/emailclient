#ifndef MAILSETTINGS_H
#define MAILSETTINGS_H

#include <QString>
#include <QStringList>
#include <settingslib.h>

class MailSettings
{
private:
    SettingsLib settings;
    bool valid = false;
public:
    MailSettings();
    QString getImapServerAddress();
    int getImapServerPort();
    QString getUserName();
    QString getPassword();
    QString getDbPath();
    int getMailNumberToFetch();
    QStringList getWatchedFolders();
    bool isValid;

};

#endif // MAILSETTINGS_H
