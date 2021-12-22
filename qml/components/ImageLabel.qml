import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: item
    height: Math.max(imageUi.implicitHeight, labelUi.implicitHeight)
    width: childrenRect.width

    property alias textColor: labelUi.color
    property alias image: imageUi.source
    property alias label: labelUi.text
    property alias fontSize: labelUi.font.pixelSize

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
    }
}
