import QtQuick

Rectangle {
    id: mailListRoot
    anchors.left: parent ? parent.left : undefined
    anchors.right: parent ? parent.right : undefined

    height: 65
    radius: 2

    property alias recipient_or_sender: recipient_or_sender.text
    property alias mailDate: mailDate.text
    property alias subject: subject.text
    border.color: "grey"
    border.width: 1


    Text {
        id: recipient_or_sender
        font.pixelSize: 15
        font.bold: true
        color: "black"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: mailDate.right
        horizontalAlignment: Text.AlignRight
        elide: Text.ElideLeft
    }

    Text {
        id: mailDate
        font.pixelSize: recipient_or_sender.font.pixelSize
        font.bold: false
        color: "dark red"
        anchors.left: parent.left
        anchors.right: recipient_or_sender.left
        anchors.top: parent.top
        horizontalAlignment: Text.AlignLeft
        elide: Text.ElideNone
    }

    Text {
        id: subject
        font.bold: false
        font.pixelSize: 15
        color: "dark blue"
        anchors.top: recipient_or_sender.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        elide: Text.ElideRight
    }
}
