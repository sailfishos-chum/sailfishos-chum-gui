import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

Item {
    id: item

    height: Theme.paddingLarge + Math.max(image.height,
                                          title.height + categories.height + author.height +
                                          categories.anchors.topMargin + author.anchors.topMargin)
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
        text: model.packageCategories ? model.packageCategories.join(", ") : ""
        truncationMode: TruncationMode.Fade
    }

    Label {
        id: author
        anchors.left: image.right
        anchors.leftMargin: Theme.paddingLarge
        anchors.right: stickers.left
        anchors.rightMargin: Theme.paddingLarge
        anchors.top: categories.bottom
        anchors.topMargin: visible ? _textMargin : 0
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
        height: visible ? implicitHeight : 0
        text: model.packagePackager ? model.packagePackager : (model.packageDeveloper ? model.packageDeveloper : "")
        truncationMode: TruncationMode.Fade
    }

    Item {
        id: stickers
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge/2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge/2
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        width: Math.max(cli.visible ? cli.width : 0,
                        stars.visible ? stars.width : 0,
                        status.visible ? status.width : 0)

        Image {
            id: status
            anchors.right: parent.right
            anchors.top: parent.top
            source: model.packageUpdateAvailable ? "image://theme/icon-s-update" :
                                                   "image://theme/icon-s-installed"
            visible: model.packageInstalled || model.packageUpdateAvailable
        }

        Image {
            id: cli
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.iconSizeSmall
            cache: true; smooth: false
            fillMode: Image.PreserveAspectFit
            source: "../../icons/icon-s-consoleapplication.svg"
            visible: model.packageType === ChumPackage.PackageApplicationConsole
        }

        Row {
            id: stars
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: Math.max(starsLab.height, starsImage.height)
            spacing: Theme.paddingSmall
            visible: model.packageStarsCount >= 0

            Label {
                id: starsLab
                anchors.verticalCenter: parent.verticalCenter
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
                text: model.packageStarsCount
            }

            Image {
                id: starsImage
                anchors.verticalCenter: parent.verticalCenter
                source: "image://theme/icon-s-favorite"
            }
        }
    }
}
