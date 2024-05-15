import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    
    Text {
        text: "Hello World from dynamically linked Qt Quick!"
        font.pointSize: 12
        anchors.verticalCenter: parent.verticalCenter
    }
}