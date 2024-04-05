import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true

    Text {
        text: "Click To Quit"
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            deleter.quit();
        }
    }
}
