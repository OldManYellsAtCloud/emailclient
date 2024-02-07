#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include <QQmlEngine>
#include <QAbstractListModel>
#include "mailengine.h"
#include "models/mailmodel.h"
#include "models/foldermodel.h"

class ModelFactory: public QObject
{
    Q_OBJECT
    QML_ELEMENT
    std::shared_ptr<MailEngine> mengine;
    std::unique_ptr<MailModel> mmodel;
    std::unique_ptr<FolderModel> fmodel;

public:
    ModelFactory();
    Q_INVOKABLE QAbstractListModel* getFolderModel();
    Q_INVOKABLE QAbstractListModel* getMailModel();
};

#endif // MODELFACTORY_H
