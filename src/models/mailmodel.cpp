#include "models/mailmodel.h"
#include "utils.h"

MailModel::MailModel(QObject *parent)
    : QAbstractListModel{parent}
{}

void MailModel::exportBody(int idx)
{
    std::string bodyContent = mails.at(idx).getBody();
    bodyContent = decodeBase64Sections(bodyContent);

    QFile f{BODY_FILE_PATH};
    f.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream qts(&f);
    qts << "<pre>";
    qts << QString::fromStdString(bodyContent);
    qts << "</pre>";
    f.close();
}

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
    busyLoading = true;
    emit busyLoadingChanged();
    mengine->fetchMailsFromWatchedFolders();
    busyLoading = false;
    emit busyLoadingChanged();
}

void MailModel::fetchNewMailsFromFolder(QString folder)
{
    LOG("Fetching new mails from {} folder", folder.toStdString());
    busyLoading = true;
    emit busyLoadingChanged();
    mengine->fetchNewMails(folder.toStdString());
    busyLoading = false;
    emit busyLoadingChanged();
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
        //exportBody(index.row());
        //return QString::fromStdString(s);
        return QVariant();
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

bool MailModel::isBusyLoading()
{
    return busyLoading;
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
