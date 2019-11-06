import QtQuick 2.0

Image {
    property int index: -1
    asynchronous:true
    fillMode: Image.PreserveAspectFit
    width: 200

    signal clicked();

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:  {
            parent.opacity = .3
        }
        onExited:   {
            parent.opacity = 1
        }
        onClicked:{
            console.log(parent.objectName + " clicked.")
            parent.clicked()
        }
    }
}
