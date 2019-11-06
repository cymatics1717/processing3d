import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.0

ApplicationWindow {
    id: root
    property real ratio: .8
    property color ccolor: "#123456"
    visible: true
    width: Screen.width*ratio
    height: Screen.height*ratio
    title: qsTr("Stack")
    property var pagemodel: ListModel {
        ListElement { title: qsTr("点云/全景"); source: "Page3Form.qml"; selectFolder:false}
        ListElement { title: qsTr("地图"); source: "Page6Form.qml"; selectFolder:false }
    }

    FileDialog {
          id: fileDialog
//          nameFilters: ["Image files (*.jpg *JPG *.png *PNG *jpeg *.JPEG *.bmp *.BMP *gif *.GIF)","All(*.*)"]
          Component.onCompleted: visible = false
    }

    Timer {
        id:ticker
        interval: 1000; running: true; repeat: true
        onTriggered: {
            var tmp = new Date().toLocaleString(Qt.locale(),"yyyy/MM/dd hh:mm:ss.zzz")
            timestamp.banner = tmp
            root.ccolor = Qt.rgba(Math.random(),Math.random(),Math.random(),1);
        }
    }

    Drawer {
        id: drawer
//        y: headerbar.height
        width: root.width * 0.2
        height: root.height

        Column {
            anchors.fill: parent
            Repeater{
                model:root.pagemodel
                ItemDelegate {
                    text: title
                    width: parent.width
                    onClicked: {
                        stackView.push(source,{"title":title})
                        drawer.close()
                    }
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: "HomeForm.qml"
        anchors.fill: parent
    }

    header: ToolBar {
        id:headerbar
        focus: true
        contentHeight: toolButton.implicitHeight
        background:Rectangle{
            color: ccolor
//            gradient: stackView.currentItem.grad
            Behavior on color {
                ColorAnimation { duration: ticker.interval }
            }
        }

        ToolButton {
            id: toolButton
            background:Rectangle{
                color: "transparent"
            }
            contentItem: Text {
                text: toolButton.text
                font: toolButton.font
                opacity: toolButton.hovered ? .3 : 1
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        ToolButton {
            id:fileButton
            visible: stackView.depth==2
            text: qsTr("打开"+stackView.currentItem.title)
            anchors.margins: 10
            anchors.left: toolButton.right
            onClicked: {
                fileDialog.selectFolder = stackView.currentItem.selectFolder
                fileDialog.title = text;
                fileDialog.visible = true;
            }
        }

        Label {
            id: title
            text: stackView.currentItem.title
            anchors.centerIn: parent
            color: "white"
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            id:timestamp
            property alias banner: inner.text
            property alias ccolor: inner.color
            background:Rectangle{
                color: "transparent"
            }
            focusPolicy: Qt.NoFocus
            contentItem: Text{
                id:inner
                color: "white"
                font.pixelSize: headerbar.font.pixelSize
            }
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            onClicked: {
            }
        }
        Keys.onPressed: {
            console.log('pressed Key '+ event.text);
            if (event.key === Qt.Key_Return || event.key === Qt.Key_F) {
                root.visibility === Window.FullScreen ? root.showNormal() : root.showFullScreen()
            } else if (event.key === Qt.Key_Backspace||event.key === Qt.Key_Escape) {
                stackView.pop();
            }  else if (event.key === Qt.Key_Q) {
                Qt.quit();
            }
        }
    }

    footer: ToolBar {
        id: foo
        height:  20
        Label {
            id:corner
            anchors.left: parent.left
        }
    }

}
