#include "models/mailmodel.h"
#include "utils.h"

MailModel::MailModel(QObject *parent)
    : QAbstractListModel{parent}
{}

MailModel::MailModel(std::shared_ptr<MailEngine> me, QObject *parent): QAbstractListModel{parent}
{
    mengine = me;
    folder = "INBOX";

    m_roleNames[MailModel::subjectRole] = "subject";
    m_roleNames[MailModel::bodyRole] = "body";
    m_roleNames[MailModel::dateRole] = "date";
    m_roleNames[MailModel::fromRole] = "from";

    connect(mengine.get(), &MailEngine::newMailFetched, this, &MailModel::newMailFetched);
}

void MailModel::setFolder(QString newFolder)
{
    clearModel();
    folder = newFolder;

    int numberOfMails = mengine->countCachedMails(folder.toStdString());

    emit beginInsertRows(QModelIndex(), 0, numberOfMails - 1);
    mails = mengine->fetchCachedMailsFromFolder(folder.toStdString());
    emit endInsertRows();
}

void MailModel::fetchNewMails()
{
    LOG("Fetching new mails...");
    mengine->fetchMailsFromWatchedFolders();
}

void MailModel::setUnreadMailFlag()
{
    LOG("Setting unread mail flag for watched folders...");
    mengine->setUnreadFlagForWatchedFolders();
}

int MailModel::rowCount(const QModelIndex &parent) const
{
    return mails.size();
}

QVariant MailModel::data(const QModelIndex &index, int role) const
{
    std::string s;
    if (index.row() < 0 || index.row() >= mails.size())
        return QVariant();

    int idx = index.row();
    mail m = mails.at(idx);

    switch(role){
    case MailModel::subjectRole:
        s = m.getSubject();
        s = decodeBase64Sections(s);
        return QString::fromStdString(s);
    case MailModel::bodyRole:
        s = m.getBody();
        s = decodeBase64Sections(s);
        return QString::fromStdString(s);
    case MailModel::fromRole:
        s = m.getFrom();
        s = decodeBase64Sections(s);
        return QString::fromStdString(s);
    case MailModel::dateRole:
        return m.getDate();
    }

    return QVariant();
}

QHash<int, QByteArray> MailModel::roleNames() const
{
    return m_roleNames;
}

void MailModel::clearModel()
{
    emit beginRemoveRows(QModelIndex(), 0, mails.size() - 1);
    mails.clear();
    emit endRemoveRows();
}

void MailModel::newMailFetched(std::string newMailFolder)
{
    LOG("New mail fetched in folder {}", newMailFolder);
    if (folder.toStdString() == newMailFolder) {
        clearModel();
        int numberOfMails = mengine->countCachedMails(folder.toStdString());

        emit beginInsertRows(QModelIndex(), 0, numberOfMails - 1);
        mails = mengine->fetchCachedMailsFromFolder(folder.toStdString());
        emit endInsertRows();
    }
}
