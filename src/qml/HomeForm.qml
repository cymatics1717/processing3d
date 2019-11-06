import QtQuick 2.12
import QtQuick.Controls 2.5
import "."

SPage {
    id: page
    title: qsTr("主页")

    Row{
        id:container
        y: page.height*.2
//        anchors.top: info.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: page.width/20

        Repeater{
            model:root.pagemodel
            TButton {
                text: qsTr(title)
                width: page.width/10
                radius: width/20
                height: width
                onClicked:{
                    stackView.push(source,{"title":title})
                }
            }
        }
    }

    Label {
        id:info
        text: qsTr("本软件用来预览全景图片和点云.\n 'F'/Enter to toggle fullscreen.\n 'q' to quit application")
        color: "white"
//        anchors.top: container.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

}
