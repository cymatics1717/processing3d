import QtQuick 2.12
import QtQuick.Controls 2.5
import "."

SPage {
    id: page
    title: qsTr("主页")

    Row{
        id:container
//        y: page.height*.2
//        anchors.top: info.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: page.width/20

        Repeater{
            model:root.pagemodel
            TButton {
                text: qsTr(title)
                width: page.width/20
                radius: width/20
                height: width
                onClicked:{
                    stackView.push(source,{"title":title})
                }
            }
        }
    }

    TButton {
        id: test
        text: qsTr("测试")
        width: parent.width/15
        height: parent.height/15
        anchors.top: container.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            backend.testFile("");
//            backend.testFile("/home/wayne/3d/github/PDAL/build/test/data/pipeline/splitter.json");
        }
    }

    Label {
        id:info
        text: qsTr("本软件用来预览全景图片和点云.\n 'F'/Enter to toggle fullscreen.\n 'q' to quit application")
        color: "white"
        anchors.top: test.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    Component.onCompleted: {
//        backend.testFile("");
    }
}
