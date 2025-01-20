import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: processingDialog

    property string title: ""
    property string status: ""

    signal aborted()

    title: status
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

        standardButtons: Dialog.Abort

        ColumnLayout {
            id: layout

            anchors.fill: parent

            Label {
                text: processingDialog.status
                font.pixelSize: 14
            }

            Item { Layout.fillHeight: true }
        }

        onRejected: {
            processingDialog.aborted()
        }
    }
}
