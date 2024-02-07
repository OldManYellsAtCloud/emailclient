#include "models/foldermodel.h"
#include "loglibrary.h"

FolderModel::FolderModel() {
    LOG("Folder model {:s} called", __FUNCTION__);
    me = std::make_shared<MailEngine>();
    folders = me->folderList().getFolderList();

    m_roleNames[FolderModel::folderNameRole] = "folderName";
}

FolderModel::FolderModel(std::shared_ptr<MailEngine> mptr)
{
    LOG("Folder model {:s} called, mailengine ptr", __FUNCTION__);
    me = mptr;
    folders = me->folderList().getFolderList();
    m_roleNames[FolderModel::folderNameRole] = "folderName";
}

int FolderModel::rowCount(const QModelIndex &parent) const
{
    return folders.size();
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > folders.size() - 1)
        return QVariant();

    switch(role){
    case FolderModel::folderNameRole:
        return QString::fromStdString(folders.at(index.row()));
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FolderModel::roleNames() const
{
    return m_roleNames;
}
