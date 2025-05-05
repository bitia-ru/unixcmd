import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: aboutDialog

    flags: Qt.Dialog | Qt.Modal | Qt.WindowStaysOnTopHint | Qt.MSWindowsFixedSizeDialogHint

    minimumHeight: dialog.height
    maximumHeight: minimumHeight
    height: minimumHeight

    minimumWidth: dialog.width
    maximumWidth: minimumWidth
    width: minimumWidth

    Dialog {
        id: dialog

        visible: true

        width: layout.implicitWidth + 50
        height: layout.implicitHeight + (footer ? footer.height : 0)

        standardButtons: Dialog.Ok

        ColumnLayout {
            id: layout

            anchors.fill: parent

            Label {
                text: "UnixCMD"

                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: 14
                font.bold: true
            }

            Label {
                text: "Version 0.0.1"

                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: 12
            }

            Label {
                text: "Copyright © 2025 Artem Levenkov. All rights reserved."

                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: 12
            }

            Item { Layout.fillHeight: true }
        }

        onAccepted: aboutDialog.close()
        onRejected: aboutDialog.close()
    }
}
