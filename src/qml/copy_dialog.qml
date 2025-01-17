import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    title: "Copying files"
    flags: Qt.Dialog | Qt.Modal | Qt.WindowStaysOnTopHint
    minimumWidth: 500
    height: dialog.height

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

                Layout.fillWidth: true
                placeholderText: qsTr("Destination path")
            }

            Item { Layout.fillHeight: true }
        }
    }
}
