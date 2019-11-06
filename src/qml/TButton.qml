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
}
