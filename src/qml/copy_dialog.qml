import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    title: "Copying files"
    flags: Qt.Dialog | Qt.Modal | Qt.WindowStaysOnTopHint
    minimumWidth: 500

    Dialog {
        visible: true
        width: parent.width
        height: parent.height
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Label {
                text: qsTr("Copy file to:")
                font.pixelSize: 14
            }

            TextField {
                id: destination
                Layout.fillWidth: true
                placeholderText: qsTr("Destination path")
            }
        }
    }
}