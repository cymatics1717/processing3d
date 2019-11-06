import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12

import SceneGraphRendering 1.0


SPage {
    property bool selectFolder: false
    property var cameraView: ListModel {
        ListElement { title: qsTr("上视图"); img: "qrc:/res/ccViewZpos.png"; oritation:VTKScene.TopView }
        ListElement { title: qsTr("下视图"); img: "qrc:/res/ccViewZneg.png"; oritation:VTKScene.BottomView}
        ListElement { title: qsTr("前视图"); img: "qrc:/res/ccViewYpos.png"; oritation:VTKScene.FrontView}
        ListElement { title: qsTr("后视图"); img: "qrc:/res/ccViewYneg.png"; oritation:VTKScene.BackView}
        ListElement { title: qsTr("左视图"); img: "qrc:/res/ccViewXpos.png"; oritation:VTKScene.LeftView}
        ListElement { title: qsTr("右视图"); img: "qrc:/res/ccViewXneg.png"; oritation:VTKScene.RightView}
        ListElement { title: qsTr("正等轴(前)"); img: "qrc:/res/ccViewIso1.png"; oritation:VTKScene.ISOFrontView}
        ListElement { title: qsTr("正等轴(后)"); img: "qrc:/res/ccViewIso2.png"; oritation:VTKScene.ISOBackView}
        ListElement { title: qsTr("归零"); img: "qrc:/res/ccViewIso1.png"; oritation:VTKScene.OriginView}
    }

    VTKScene {
        id: scene
        anchors.fill: parent
        focus: true

        onToggleWindow:{
            if(root.visibility === Window.FullScreen){
                root.showNormal()
            } else {
                root.showFullScreen()
            }
        }
        onStepback: {
            stackView.pop();
        }
        onToggleShow: {
            rect.visible = ! rect.visible
        }

        Component.onCompleted: {
            scene.camera_Orientation = VTKScene.LeftView
        }
    }

    ScrollView {
        id:rect
        clip: true
        visible: false
        anchors{
            bottom: scene.bottom
            horizontalCenter: scene.horizontalCenter
        }
        width: parent.width
        height: parent.height/10

        ListView {
            id: viewlist
            anchors.fill: parent
            orientation: Qt.Horizontal
            spacing: 10

            model: cameraView
            delegate: TButton {
                color: "grey"
                width:height
                height: rect.height
                radius: height/2
                text: title
                src: img
                onClicked:{
                    console.log(title + " clicked.")
                    scene.camera_Orientation = oritation
                    corner.text = title
                }
            }
        }
    }


    Connections {
        target: fileDialog
        onAccepted: {
            console.info(fileDialog.fileUrl)
            scene.openFile(fileDialog.fileUrl)
            rect.visible = true
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
