#ifndef MAILMODEL_H
#define MAILMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <memory>
#include "mailengine.h"

class MailModel : public QAbstractListModel
{
    Q_OBJECT
    std::shared_ptr<MailEngine> mengine;
    QString folder;
    QHash<int, QByteArray> m_roleNames;
    std::vector<mail> mails;

public:
    explicit MailModel(std::shared_ptr<MailEngine> me, QObject *parent = nullptr);
    MailModel(QObject *parent);
    Q_INVOKABLE void setFolder(QString newFolder);
    Q_INVOKABLE void fetchNewMails();
    Q_INVOKABLE void setUnreadMailFlag();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void clearModel();

    enum RoleNames {
        subjectRole = Qt::UserRole,
        fromRole = Qt::UserRole + 1,
        dateRole = Qt::UserRole + 2,
        bodyRole = Qt::UserRole + 3
    };

public slots:
    void newMailFetched(std::string newMailFolder);

};

#endif // MAILMODEL_H
