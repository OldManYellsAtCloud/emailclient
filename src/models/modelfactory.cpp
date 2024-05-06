#include "models/modelfactory.h"

ModelFactory::ModelFactory() {
    mengine = std::make_shared<MailEngine>();

    fmodel = std::make_unique<FolderModel>(mengine);
    mmodel = std::make_unique<MailModel>(mengine);
}

QAbstractListModel *ModelFactory::getFolderModel()
{
    return fmodel.get();
}

QAbstractListModel *ModelFactory::getMailModel()
{
    return mmodel.get();
}
