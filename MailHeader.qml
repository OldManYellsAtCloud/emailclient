import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

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

    MessageDialog {
        id: indexTest
        title: "Not implemented"
        buttons: Dialog.Ok
        onAccepted: {}
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
        id: recipient_or_sender
        font.pixelSize: 15
        font.bold: true
        color: "black"
        anchors.top: mailDate.bottom
        anchors.right: parent.right
        anchors.left: mailDate.right
        horizontalAlignment: Text.AlignRight
        elide: Text.ElideLeft
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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            model.expo
            indexTest.title = "whatev " + model.index
            indexTest.open()
        }
    }
}
