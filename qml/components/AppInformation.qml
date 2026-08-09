import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

MouseArea {
    id: infoItem

    property bool enableExpansion: true
    property int  shrunkHeight: 500
    property var  pkg
    property bool developerShown: false

    property bool _expanded: !_expansionEnabled
    property bool _expansionEnabled: enableExpansion && content.implicitHeight > shrunkHeight

    function aiScore(key) {
        switch (key) {
            case "H":
                //% "Human-written code only"
                //: AI rating, be brief in translation
                return qsTrId("chum-pkg-ai-code-desc-h")
                break
            case "A":
                //% "Human-written code, AI used elsewhere"
                //: AI rating, be brief in translation
                return qsTrId("chum-pkg-ai-code-desc-a")
                break
            case "B":
                //% "AI-assisted code, human-reviewed"
                //: AI rating, be brief in translation
                return qsTrId("chum-pkg-ai-code-desc-b")
                break
            case "C":
                //% "AI-written code"
                //: AI rating, be brief in translation
                return qsTrId("chum-pkg-ai-code-desc-c")
                break
            case "V":
                //% "Vibe-coded"
                //: AI rating, be brief in translation
                return qsTrId("chum-pkg-ai-code-desc-v")
                break
            case "X":
                //% "Not disclosed"
                //: AI rating, be brief in translation
                return qsTrId("chum-pkg-ai-code-desc-not-disclosed")
                break
            default:
                //% "unknown/not specified"
                //: AI rating has not been set
                return qsTrId("chum-pkg-ai-code-desc-unknown")
        }
    }

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
            //% "AI Rating:"
            label: qsTrId("chum-pkg-ai-code")
            value: infoItem.aiScore(pkg.aiCode)
        }

        Label {
            width: parent.width
            //% "AI Notes:"
            text: qsTrId("chum-pkg-ai-desc-label")
            color: Theme.secondaryHighlightColor
            linkColor: Theme.highlightColor
            visible: pkg.aiDescription || aiDescMD.fetching || aiDescMD.text
            font.pixelSize: Theme.fontSizeSmall
        }

        Label {
            width: parent.width
            text: pkg.aiDescription
            color: Theme.highlightColor
            linkColor: Theme.primaryColor
            visible: pkg.aiDescription
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
            font.pixelSize: Theme.fontSizeSmall
        }

        LabelMarkdown {
            id: aiDescMD
            height: fetching ? Theme.paddingLarge + implicitHeight : implicitHeight
            width: parent.width
            url: pkg.aiDescriptionMDUrl
            color: infoItem.pressed ? Theme.secondaryHighlightColor : Theme.secondaryColor
            linkColor: Theme.highlightColor
            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link)
        }

        Item {
            id: aispacer
            height: Theme.paddingLarge
            width: parent.width
            visible: aiDescMD.text
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
