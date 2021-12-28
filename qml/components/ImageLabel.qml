import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: item
    height: visible ? Math.max(imageUi.implicitHeight, labelUi.implicitHeight) : 0
    width: visible ? imageUi.width + labelUi.anchors.leftMargin + labelUi.implicitWidth : 0

    property alias fontSize: labelUi.font.pixelSize
    property alias image: imageUi.source
    property alias label: labelUi.text
    property alias textColor: labelUi.color

    Image {
        id: imageUi
        anchors.left: item.left
        anchors.verticalCenter: item.verticalCenter
    }

    Label {
        id: labelUi
        anchors.left: imageUi.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.verticalCenter: item.verticalCenter
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
        truncationMode: TruncationMode.Fade
    }
}
