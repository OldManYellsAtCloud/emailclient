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
    QString getApplicationUser();
    int getMailNumberToFetch();
    QStringList getWatchedFolders();
    int getImapRequestDelay();
    bool isValid;

};

#endif // MAILSETTINGS_H
