import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

MouseArea {
    id: infoItem

    property bool enableExpansion: true
    property int  shrunkHeight: 500
    property var  pkg
    property var  mtime
    property bool developerShown: false

    property bool _expanded: !_expansionEnabled
    property bool _expansionEnabled: enableExpansion && content.implicitHeight > shrunkHeight

    clip: true
    enabled: _expansionEnabled
    height: content.height + (dots.visible ? dots.height : 0)
    width: parent.width
    propagateComposedEvents: true
    onClicked: _expanded = !_expanded

    Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

    Column {
        id: content
        height: (!_expansionEnabled || _expanded) ? implicitHeight : shrunkHeight - dots.height
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        LabelMarkdown {
            id: descMD
            height: fetching ? Theme.paddingLarge + implicitHeight : implicitHeight
            width: parent.width
            url: pkg.descriptionMDUrl
            color: infoItem.pressed ? Theme.highlightColor : Theme.primaryColor
            linkColor: Theme.highlightColor
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }

        Label {
            width: parent.width
            text: pkg.description
            color: infoItem.pressed ? Theme.highlightColor : Theme.primaryColor
            linkColor: Theme.highlightColor
            visible: descMD.fetching || !descMD.text
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }

        Item {
            id: spacer
            height: Theme.paddingLarge
            width: parent.width
        }

        ChumDetailItem {
            visible: pkg.developer && infoItem.developerShown
            //% "Developer:"
            label: qsTrId("chum-pkg-developer")
            value: pkg.developer
        }

        ChumDetailItem {
            visible: pkg.installed
            //% "Installed version:"
            label: qsTrId("chum-pkg-installed-version")
            value: pkg.installedVersion
        }

        ChumDetailItem {
            //% "Available version:"
            label: qsTrId("chum-pkg-available-version")
            value: pkg.availableVersion
            visible: pkg.availableVersion
        }

        ChumDetailItem {
            //% "Package name:"
            label: qsTrId("chum-pkg-package-name")
            value: pkg.packageName
            visible: pkg.packageName
        }

        ChumDetailItem {
            //% "Package last updated:"
            label: qsTrId("chum-pkg-package-mtime")
            value: mtime
            visible: mtime
        }

        ChumDetailItem {
            //% "Download size:"
            label: qsTrId("chum-pkg-download-size")
            value: Format.formatFileSize(pkg.size)
            visible: pkg.size
        }

        ChumDetailItem {
            //% "License:"
            label: qsTrId("chum-pkg-license")
            value: pkg.license
            visible: pkg.license
        }

        ChumDetailItem {
            visible: !!pkg.url
            text: '<font color="%1">%3</font> <font color="%2"><a href="%4">%4</a></font>'
            .arg(Theme.secondaryHighlightColor)
            .arg(Theme.primaryColor)
            //% "Homepage:"
            .arg(qsTrId("chum-pkg-link"))
            .arg(pkg.url)

            onLinkActivated: Qt.openUrlExternally(link)
        }

        ChumDetailItem {
            visible: !!pkg.packagingUrl
            text: '<font color="%1">%3</font> <font color="%2"><a href="%4">%4</a></font>'
            .arg(Theme.secondaryHighlightColor)
            .arg(Theme.primaryColor)
            //% "Packaging repository:"
            .arg(qsTrId("chum-pkg-packaging-link"))
            .arg(pkg.packagingUrl)

            onLinkActivated: Qt.openUrlExternally(link)
        }
    }

    Image {
        id: dots
        anchors.top: content.bottom
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        source: "image://theme/icon-lock-more"
        visible: !_expanded
    }

    OpacityRampEffect {
        sourceItem: content
        enabled: !_expanded
        direction: OpacityRamp.TopToBottom
    }
}
