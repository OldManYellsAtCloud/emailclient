import QtQuick

Item {
    id: root
    ListView {
        id: mailListView
        anchors.fill: parent
        model: modelFactory.getMailModel()
        delegate: MailHeader {
            recipient_or_sender: model.from
            subject: model.subject
            mailDate: new Date(model.date * 1000).toLocaleString(Qt.locale("de_DE"), "yyyy-MM-dd hh:mm")
        }
    }
}
