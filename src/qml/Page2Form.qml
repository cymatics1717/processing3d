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
        id: scene1
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
            console.log("")
            rect.visible = ! rect.visible
            animator.restart()
        }
        onClicked: {
            console.log("")
            animator.restart()
        }
        onMessage: {
            corner.text = msg
        }
        onSceneCompleted:{
            console.log("++++++++++++++++++++===")
            scene1.camera_Orientation = VTKScene.FrontView
//            scene1.openFile("/home/wayne/3d/red_pepper.stl");
            scene1.openFile("/home/wayne/3d/00001-pano.jpg");
            scene1.openFile("/home/wayne/3d/Cloud2.ply");
        }
    }

    ScrollView {
        id:rect
        clip: true
        visible: true
        anchors{
            bottom: scene1.bottom
            horizontalCenter: scene1.horizontalCenter
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
                    scene1.camera_Orientation = oritation
                    corner.text = title
                }
            }
        }
    }


    Connections {
        target: fileDialog
        onAccepted: {
            console.info(fileDialog.fileUrl)
            scene1.openFile(fileDialog.fileUrl)
            rect.visible = true
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    NumberAnimation  {
        property quaternion start: eulerToQuaternionXYZ(0, 0,0)
        property quaternion end: /*Qt.quaternion(1,90,90,90)*/eulerToQuaternionXYZ(0, 0, 180)
        property real progress: 0
        id: animator
        target: animator
        property: "progress"
        from: 0.0
        to: 1.0
        duration: 1000
//        running: true
        loops: 1
//        loops:Animation.Infinite
        easing.type: Easing.OutInElastic;
        onProgressChanged: {
//            scene1.pose = slerp(start, end, progress)
            scene1.progress = progress
//            scene1.pose = angleAxisToQuat(animator.progress*180, Qt.vector3d(0,0,1))
            scene1.scale3D = 1.1
//            scene1.position = Qt.vector3d(progress*10,0,0)
//            console.log("progress = "+ scene1.position)

            scene1.pose = slerp(start,end,progress)
        }
        onStarted: {
            scene1.running = true
            scene1.scale3D = 1
//            scene1.position = Qt.vector3d(0,0,0)
        }
        onFinished: {
            scene1.running = false
            scene1.scale3D = 1
//            scene1.position = Qt.vector3d(0,0,0)
        }
    }

    function angleAxisToQuat(angle, axis) {
        var k = 1;
        var m = 2;
        var a = angle * Math.PI / 180.0;
        var s = m*Math.sin(a * k);
        var c = m*Math.cos(a * k);
        return Qt.quaternion(c, axis.x * s, axis.y * s, axis.z * s);
    }

    function multiplyQuaternion(q1, q2) {
        return Qt.quaternion(q1.scalar * q2.scalar - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
                             q1.scalar * q2.x + q1.x * q2.scalar + q1.y * q2.z - q1.z * q2.y,
                             q1.scalar * q2.y + q1.y * q2.scalar + q1.z * q2.x - q1.x * q2.z,
                             q1.scalar * q2.z + q1.z * q2.scalar + q1.x * q2.y - q1.y * q2.x);
    }

    function eulerToQuaternionXYZ(x, y, z) {
        var quatX = angleAxisToQuat(x, Qt.vector3d(1, 0, 0));
        var quatY = angleAxisToQuat(y, Qt.vector3d(0, 1, 0));
        var quatZ = angleAxisToQuat(z, Qt.vector3d(0, 0, 1));
        return multiplyQuaternion(multiplyQuaternion(quatX, quatY), quatZ)
    }

    function slerp(start, end, t) {

        var halfCosTheta = ((start.x * end.x) + (start.y * end.y)) + ((start.z * end.z) + (start.scalar * end.scalar));

        if (halfCosTheta < 0.0)
        {
            end.scalar = -end.scalar
            end.x = -end.x
            end.y = -end.y
            end.z = -end.z
            halfCosTheta = -halfCosTheta;
        }

        if (Math.abs(halfCosTheta) > 0.999999)
        {
            return Qt.quaternion(start.scalar + (t * (end.scalar - start.scalar)),
                                 start.x      + (t * (end.x      - start.x     )),
                                 start.y      + (t * (end.y      - start.y     )),
                                 start.z      + (t * (end.z      - start.z     )));
        }

        var halfTheta = Math.acos(halfCosTheta);
        var s1 = Math.sin((1.0 - t) * halfTheta);
        var s2 = Math.sin(t * halfTheta);
        var s3 = 1.0 / Math.sin(halfTheta);
        return Qt.quaternion((s1 * start.scalar + s2 * end.scalar) * s3,
                             (s1 * start.x      + s2 * end.x     ) * s3,
                             (s1 * start.y      + s2 * end.y     ) * s3,
                             (s1 * start.z      + s2 * end.z     ) * s3);
    }

    function getAxis(quat) {
        var tmp1 = 1.0 - quat.scalar * quat.scalar;
        if (tmp1 <= 0) return Qt.vector3d(0.0, 0.0, 1.0);
        var tmp2 = 1 / Math.sqrt(tmp1);
        return Qt.vector3d(quat.x * tmp2, quat.y * tmp2, quat.z * tmp2);
    }

    function getAngle(quat) {
        return Math.acos(quat.scalar) * 2.0 * 180.0 / Math.PI;
    }

//    Component.onCompleted: {
//        animator.start()
//    }

}
