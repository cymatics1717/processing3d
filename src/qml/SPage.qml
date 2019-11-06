import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    property color leftColor: "#090d25"
    property color rightColor: "#090d21"
    property var grad: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0; color: rightColor }
        GradientStop { position: 1; color: leftColor }
    }
    background:Rectangle{
        gradient: grad
    }

}


