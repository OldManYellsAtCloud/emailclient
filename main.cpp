#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtWebEngineQuick/QtWebEngineQuick>

#include "mailsettings.h"
#include "mailengine.h"
#include "models/foldermodel.h"
#include "models/modelfactory.h"
#include "models/mailmodel.h"
#include "dbengine.h"

#include <iostream>

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <loglibrary.h>


void checkProcessOwner(int argc, char *argv[]){
    MailSettings settings{};
    std::string applicationUser = settings.getApplicationUser().toStdString();
    auto currentUid = getuid();
    auto currentUser = getpwuid(currentUid);

    std::string currentUserName {currentUser->pw_name};

    if (currentUserName.compare(applicationUser) != 0){
        LOG("Running with incorrect user: {} Attempting to switch to application user.", currentUserName);
        auto appUserUid = getpwnam(applicationUser.c_str());

        if (!appUserUid){
            ERROR("Could not find application user! Error: {}", strerror(errno));
            exit(1);
        }

        if(setuid(appUserUid->pw_uid) < 0){
            ERROR("Could not set UID to application user! Error: {}", strerror(errno));
            exit(1);
        }

        if (setgid(appUserUid->pw_gid) < 0){
            ERROR("Could not set GID to application user's group! Error: {}", strerror(errno));
            exit(1);
        }

        execv(argv[0], argv);
    } else {
        LOG("Already running as application user.");
    }
}


int main(int argc, char *argv[])
{
    checkProcessOwner(argc, argv);
    qmlRegisterType<MailEngine>("sgy.pine.mail", 1, 0, "MailEngine");
    qmlRegisterType<FolderModel>("sgy.pine.mail", 1, 0, "FolderModel");
    qmlRegisterType<ModelFactory>("sgy.pine.mail", 1, 0, "ModelFactory");
    qmlRegisterType<MailModel>("sgy.pine.mail", 1, 0, "MailModel");

    QtWebEngineQuick::initialize();
    // WebKit is issuing a warning about shared opengl contexts
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/mailclient/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);


    return app.exec();
}
