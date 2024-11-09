import QtQuick 6.0
import QtQuick.Window 6.0
import QtQuick.Controls 6.0
import QtQml 6.0

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