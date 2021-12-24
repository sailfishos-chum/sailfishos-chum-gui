import QtQuick 2.0
import Sailfish.Silica 1.0

MouseArea {
    property bool _expanded: false
    property int  shrunkHeight: 500
    property bool enableExpansion: true
    property var pkg

    id: infoItem
    width: parent.width
    height: content.height + (dots.visible ? dots.height : 0)
    clip: true
    propagateComposedEvents: true
    enabled: enableExpansion
    onClicked: _expanded = !_expanded

    Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

    Column {
        id: content
        height: !enableExpansion || _expanded || shrunkHeight > implicitHeight ? implicitHeight : shrunkHeight
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        Item {
            width: parent.width
            height: bodyLabel.height

            Label {
                id: bodyLabel
                width: parent.width
                //maximumLineCount: _expanded ? 0 : 7
                text: pkg.description
                color: infoItem.pressed ? Theme.highlightColor : Theme.primaryColor
                linkColor: Theme.highlightColor
                wrapMode: Text.WordWrap
                onLinkActivated: openLink(link)
            }
        }

        Item {
            id: spacer
            visible: _expanded
            height: Theme.paddingMedium
        }

        ChumDetailItem {
            id: installedVersionItem
            visible: pkg.installed
            //% "Installed version"
            label: qsTrId("chum-pkg-installed-version")
            value: pkg.installedVersion
        }

        ChumDetailItem {
            id: availableVersionItem
            //% "Available version"
            label: qsTrId("chum-pkg-available-version")
            value: pkg.availableVersion
            visible: pkg.availableVersion
        }

        ChumDetailItem {
            //% "Download size"
            label: qsTrId("chum-pkg-download-size")
            value: Format.formatFileSize(pkg.size)
            visible: pkg.size
        }

        ChumDetailItem {
            //% "License"
            label: qsTrId("chum-pkg-license")
            value: pkg.license
            visible: pkg.license
        }

        ChumDetailItem {
            visible: !!pkg.url
            text: '<font color="%1">%3</font> <font color="%2"><a href="%4">%4</a></font>'
            .arg(Theme.secondaryHighlightColor)
            .arg(Theme.primaryColor)
            //% "Link"
            .arg(qsTrId("chum-pkg-link"))
            .arg(pkg.url)

            onLinkActivated: Qt.openUrlExternally(link)
        }
    }

    Image {
        id: dots
        anchors.top: content.bottom
        anchors.right: parent.right
        anchors.rightMargin: Theme.horizontalPageMargin
        source: "image://theme/icon-lock-more"
        visible: !_expanded && enableExpansion
    }

    OpacityRampEffect {
        sourceItem: content
        enabled: !_expanded && enableExpansion
        direction: OpacityRamp.TopToBottom
    }
}

