import QtQuick 2.12
import QtQuick.Controls 2.5

Rectangle {
//    focus: false
    property string text
//    property color lightColor: "grey"
    property alias src: icon.source
    property color leftColor: "#2e3a60"
    property color rightColor: "#090d21"
    property var grad: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0; color: leftColor }
        GradientStop { position: 1; color: rightColor }
    }

    signal clicked();

    border.color: "grey"
    border.width: 1
    radius:       0.01 * height
//    opacity:      enabled? 1: 0.3

//    color: "#111111"

    gradient: grad

    Image {
        id: icon
        asynchronous: true
        anchors.horizontalCenter:  parent.horizontalCenter
        anchors.verticalCenter:   parent.verticalCenter
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent

    }

    Text {
        text: parent.text
        color: "white"
        font.pixelSize: 0.2 * parent.height
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:  {
            parent.opacity = .3
        }
        onExited:   {
            parent.opacity = 1
        }
//        onPressed:  fenter()
//        onReleased: fleave()
//        onCanceled: fleave()
        onClicked:{
            console.log(parent.objectName + " clicked.")
            parent.clicked()
        }
    }

    transform: Rotation {
        id: rot
    }

    NumberAnimation  {
        property quaternion start: Qt.quaternion(100,180,180,180)
        property quaternion end: Qt.quaternion(0,0,0,0)
        property real progress: 0
        id: animator
        target: animator
        property: "progress"
        from: 0.0
        to: 1.0
        duration: 1000
        running: true
//        loops:Animation.Infinite
        onProgressChanged: {
            var result = slerp(start, end, progress)
//             console.log(result)
            rot.angle = result.scalar
            rot.axis.x = result.x
            rot.axis.y = result.y
            rot.axis.z = result.z
        }
    }


    function fenter(){
//        console.log(typeof(JSON.stringify(popup.content.source)))
        page.opacity = 0.1
        if(objectName && objectName.length>0){
//            console.log(popup.content.item+"++++++++++++"+objectName+"++"+popup.content.source)
            if(JSON.stringify(popup.content.source).includes(objectName) ){
                opacity = 1
            } else {
                opacity = .1
            }
        }
    }

    function fleave(){
        if(objectName && objectName.length>0){
            opacity = 1
            page.opacity = 1
        }
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
}
