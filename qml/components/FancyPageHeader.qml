import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property alias title: header.title
    property alias description: header.description
    property alias author: auth.text
    property alias iconSource: image.source
    property bool  packagerShown: false

    width: parent.width
    height: Math.max(column.height, image.height + image.anchors.topMargin)

    Column {
        id: column
        anchors {
            left: parent.left
            right: image.visible ? image.left : parent.right
            rightMargin: image.visible ? Theme.paddingLarge : Theme.horizontalPageMargin
        }

        PageHeader {
            id: header
            rightMargin: 0
            width: parent.width
        }

        Label {
            anchors {
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
            }
            color: Theme.secondaryHighlightColor
            font.pixelSize: Theme.fontSizeSmall
            height: visible ? implicitHeight + Theme.paddingSmall : 0
            horizontalAlignment: implicitWidth < width ? Text.AlignRight : Text.AlignLeft
            //% "Packaged by"
            text: qsTrId("chum-page-header-packaged-by")
            visible: packagerShown
        }

        Label {
            id: auth
            anchors {
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
            }
            color: Theme.secondaryHighlightColor
            font.pixelSize: Theme.fontSizeSmall
            horizontalAlignment: implicitWidth < width ? Text.AlignRight : Text.AlignLeft
            truncationMode: TruncationMode.Fade
            visible: text
        }
    }

    Image {
        id: image
        visible: source.toString()
        anchors {
            top: parent.top
            topMargin: Theme.horizontalPageMargin + Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }
        width: Theme.iconSizeLauncher
        height: Theme.iconSizeLauncher
        fillMode: Image.PreserveAspectFit
    }
}
