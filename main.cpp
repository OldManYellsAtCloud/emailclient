#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtWebEngineQuick/QtWebEngineQuick>

#include "mailsettings.h"
#include "mailengine.h"
#include "models/foldermodel.h"
#include "models/modelfactory.h"
#include "models/mailmodel.h"
#include "dbengine.h"
#include <thread>
#include <chrono>

#include <iostream>



int main(int argc, char *argv[])
{

    qmlRegisterType<MailEngine>("sgy.pine.mail", 1, 0, "MailEngine");
    qmlRegisterType<FolderModel>("sgy.pine.mail", 1, 0, "FolderModel");
    qmlRegisterType<ModelFactory>("sgy.pine.mail", 1, 0, "ModelFactory");
    qmlRegisterType<MailModel>("sgy.pine.mail", 1, 0, "MailModel");

    QtWebEngineQuick::initialize();
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
