#ifndef MAILMODEL_H
#define MAILMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <memory>
#include "mailengine.h"

#define BODY_FILE_PATH "/tmp/emailbody.html"

class MailModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busyLoading READ isBusyLoading NOTIFY busyLoadingChanged)
    std::shared_ptr<MailEngine> mengine;
    QString folder;
    QHash<int, QByteArray> m_roleNames;
    std::vector<mail> mails;
    bool busyLoading;


public:
    explicit MailModel(std::shared_ptr<MailEngine> me, QObject *parent = nullptr);
    MailModel(QObject *parent);
    Q_INVOKABLE void setFolder(QString newFolder);
    Q_INVOKABLE void fetchNewMails();
    Q_INVOKABLE void fetchNewMailsFromFolder(QString folder);
    Q_INVOKABLE void setUnreadMailFlag();
    Q_INVOKABLE void exportBody(int idx);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void clearModel();

    bool isBusyLoading();

    enum RoleNames {
        subjectRole = Qt::UserRole,
        fromRole = Qt::UserRole + 1,
        dateRole = Qt::UserRole + 2,
        bodyRole = Qt::UserRole + 3
    };

public slots:
    void newMailFetched(std::string newMailFolder);

signals:
    void busyLoadingChanged();
};

#endif // MAILMODEL_H
