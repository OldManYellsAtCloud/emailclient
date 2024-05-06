import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import sgy.pine.mail

Window {
    width: 640
    height: 480
    visible: true

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: MailList {}
    }

    MessageDialog {
        id: notImplemented
        title: "Not implemented"
        buttons: Dialog.Ok
        onAccepted: {}
    }

    ModelFactory{
        id: modelFactory
    }

    BusyIndicator {
        running: modelFactory.getMailModel().busyLoading
    }



    Drawer {
        id: drawer
        width: 0.66 * parent.width
        height: parent.height
        Label {
            id: title
            text: "Menu"
            anchors.top: parent.top

        }

        Button {
            id: newMailButton
            anchors.top: title.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            text: "New email"
            onClicked: notImplemented.open()

        }

        ListView {
            id: drawerListview
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: newMailButton.bottom
            anchors.bottom: parent.bottom
            model: modelFactory.getFolderModel();

            delegate: FolderListDelegate {
                text: model.folderName;
            }
        }
    }

    Timer {
        interval: 1000 * 60 * 5// 5 minutes
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: {
            modelFactory.getMailModel().fetchNewMails()
            modelFactory.getMailModel().setUnreadMailFlag()
        }
    }


}

