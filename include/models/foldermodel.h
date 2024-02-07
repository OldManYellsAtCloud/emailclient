#ifndef FOLDERMODEL_H
#define FOLDERMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "mailengine.h"

class FolderModel : public QAbstractListModel
{
    Q_OBJECT
    std::vector<std::string> folders;
    std::shared_ptr<MailEngine> me;
    QHash<int, QByteArray> m_roleNames;

public:
    FolderModel();
    FolderModel(std::shared_ptr<MailEngine> mptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum RoleNames {
        folderNameRole = Qt::UserRole
    };
};

#endif // FOLDERMODEL_H
