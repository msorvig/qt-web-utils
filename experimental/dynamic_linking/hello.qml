import QtCore
import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    
    Text {
        id: headline
        text: "Hello from Qt using shared libraries!"
        font.pointSize: 14
    }
    Text {
        anchors.top: headline.bottom
        anchors.topMargin: 10
        text: "Click to load plugin"
        font.pointSize: 14
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("Text clicked!")
                pluginLoader.loadPlugin()
            }    
        }
    }
}