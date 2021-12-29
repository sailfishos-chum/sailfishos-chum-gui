import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

Item {
    id: item

    height: Theme.paddingLarge + Math.max(image.height,
                                          title.height + categories.height + developer.height +
                                          categories.anchors.topMargin + developer.anchors.topMargin)
    width: parent.width

    property bool highlighted: false

    property real _textMargin: Theme.paddingSmall/2

    Image {
        id: image
        anchors.left: item.left
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.verticalCenter: item.verticalCenter
        fillMode: Image.PreserveAspectFit
        source: model.packageIcon
        height: Theme.iconSizeLarge
        width: Theme.iconSizeLarge
    }

    Label {
        id: title
        anchors.left: image.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        anchors.top: item.top
        anchors.topMargin: Theme.paddingLarge/2
        color: parent.highlighted ? Theme.highlightColor : Theme.primaryColor
        text: model.packageName
        truncationMode: TruncationMode.Fade
    }

    Label {
        id: categories
        anchors.left: image.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.right: stickers.left
        anchors.rightMargin: Theme.paddingLarge
        anchors.top: title.bottom
        anchors.topMargin: visible ? _textMargin : 0
        color: parent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
        font.pixelSize: Theme.fontSizeSmall
        height: visible ? implicitHeight : 0
        text: model.packageCategories.join(" ")
        truncationMode: TruncationMode.Fade
        visible: text
    }

    Label {
        id: developer
        anchors.left: image.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.right: stickers.left
        anchors.rightMargin: Theme.paddingLarge
        anchors.top: categories.bottom
        anchors.topMargin: visible ? _textMargin : 0
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
        height: visible ? implicitHeight : 0
        text: model.packageDeveloper
        truncationMode: TruncationMode.Fade
        visible: text
    }

    Row {
        id: stickers
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge/2
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        height: Math.max(stars.visible ? stars.height : 0,
                         status.visible ? status.height : 0)
        spacing: Theme.paddingLarge

        ImageLabel {
            id: stars
            anchors.verticalCenter: parent.verticalCenter
            image: "image://theme/icon-s-favorite"
            label: model.packageStarsCount
            visible: model.packageStarsCount >= 0
        }

        Image {
            id: status
            anchors.verticalCenter: parent.verticalCenter
            source: model.packageUpdateAvailable ? "image://theme/icon-s-update" :
                                                  "image://theme/icon-s-installed"
            visible: model.packageInstalled || model.packageUpdateAvailable
        }
    }
}
