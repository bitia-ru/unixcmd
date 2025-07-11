import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: moveCopyDialog

    property string destination: ""
    property int operationType: 0
    property int fileCount: 1

    signal accepted(destination: string)
    signal canceled()

    title: operationType === 0 ? "Copying files" : "Moving files"
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
                text: {
                    const operation = operationType === 0 ? "Copy" : "Move";
                    const fileText = fileCount === 1 ? "file" : "files";
                    return operation + " " + fileText + " to:";
                }
                font.pixelSize: 14
            }

            TextField {
                id: destination

                focus: true

                Layout.fillWidth: true
                placeholderText: qsTr("Destination path")
                text: moveCopyDialog.destination

                onAccepted: dialog.accept()
            }

            Item { Layout.fillHeight: true }
        }

        onAccepted: accept()

        onRejected: {
            moveCopyDialog.canceled()
        }

        function accept() {
            moveCopyDialog.accepted(destination.text)
        }
    }
}
