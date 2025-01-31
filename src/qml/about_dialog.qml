import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: aboutDialog

    flags: Qt.Dialog | Qt.Modal | Qt.WindowStaysOnTopHint | Qt.MSWindowsFixedSizeDialogHint

    minimumHeight: dialog.height
    maximumHeight: minimumHeight
    height: minimumHeight

    Dialog {
        id: dialog

        visible: true

        width: parent.width
        height: layout.implicitHeight + (footer ? footer.height : 0)

        standardButtons: Dialog.Ok

        ColumnLayout {
            id: layout

            anchors.fill: parent

            Label {
                text: "About UnixCMD"
                font.pixelSize: 14
            }

            Item { Layout.fillHeight: true }
        }

        onAccepted: aboutDialog.close()
        onRejected: aboutDialog.close()
    }
}
