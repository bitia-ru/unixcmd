import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: copyDialog

    property string destination: ""

    signal accepted(destination: string)
    signal canceled()

    title: "Copying files"
    flags: Qt.Dialog | Qt.Modal | Qt.WindowStaysOnTopHint | Qt.MSWindowsFixedSizeDialogHint

    minimumWidth: 500
    minimumHeight: dialog.height
    maximumHeight: minimumHeight
    height: minimumHeight

    Dialog {
        id: dialog

        visible: true

        width: parent.width
        height: layout.implicitHeight + (footer ? footer.height : 0)

        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            id: layout

            anchors.fill: parent

            Label {
                text: qsTr("Copy file to:")
                font.pixelSize: 14
            }

            TextField {
                id: destination

                focus: true

                Layout.fillWidth: true
                placeholderText: qsTr("Destination path")
                text: copyDialog.destination
            }

            Item { Layout.fillHeight: true }
        }

        onAccepted: {
            copyDialog.accepted(destination.text)
        }

        onRejected: {
            copyDialog.canceled()
        }
    }
}
