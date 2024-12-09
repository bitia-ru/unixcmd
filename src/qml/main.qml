import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQml

ApplicationWindow
{
    visible: true
    title: "Example"
    width: 400
    height: 400

    Label {
        anchors.centerIn: parent
        text: qsTr("Hello World!")
        font.pixelSize: 14
    }
}