import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: createDirectoryDialog

    property string directoryName: ""

    signal accepted(directoryName: string)
    signal canceled()

    title: "Creating directory"
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
                text: qsTr("Enter directory name:")
                font.pixelSize: 14
            }

            TextField {
                id: directoryNameField

                focus: true

                Layout.fillWidth: true
                placeholderText: qsTr("Directory name")
                text: createDirectoryDialog.directoryName

                onAccepted: dialog.accept()
            }

            Item { Layout.fillHeight: true }
        }

        onAccepted: accept()

        onRejected: {
            createDirectoryDialog.canceled()
        }

        function accept() {
            createDirectoryDialog.accepted(directoryNameField.text)
        }
    }
}
