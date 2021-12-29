import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

CoverBackground {
    id: cover

    Image {
        id: image
        anchors.centerIn: parent
        height: width/sourceSize.width * sourceSize.height
        scale: Image.PreserveAspectFit
        smooth: true
        source: "../../icons/sailfishos-chum-gui.svg"
        width: parent.width  / 2
    }

    Label {
        id: label
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: image.bottom
        anchors.topMargin: Theme.paddingMedium
        text: Chum.busy ?
                  //% "Busy"
                  qsTrId("chum-busy") :
                  ""
    }
}
