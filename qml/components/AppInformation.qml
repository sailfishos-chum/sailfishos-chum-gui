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

    function aiBadge(key) {
        switch (key) {
            case "H":
                return Qt.resolvedUrl("image://theme/AI-score-H")
                break
            case "A":
                return Qt.resolvedUrl("image://theme/AI-score-A")
                break
            case "B":
                return Qt.resolvedUrl("image://theme/AI-score-B")
                break
            case "C":
                return Qt.resolvedUrl("image://theme/AI-score-C")
                break
            case "V":
                return Qt.resolvedUrl("image://theme/AI-score-V")
                break
            case "X":
                return Qt.resolvedUrl("image://theme/AI-score-will-not-tell")
                break
            default:
                return Qt.resolvedUrl("image://theme/AI-score-unknown")
        }
    }

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
                //% "Not specified"
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

        Row { id: badgeRow
            width: parent.width
            Image { id: aiBadgeIcon
                visible: infoItem.aiBadge(pkg.aiCode)
                source: infoItem.aiBadge(pkg.aiCode)
                width: Theme.itemSizeLarge
                height: Theme.iconSizeLarge
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
            }
            DetailItem {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - aiBadgeIcon.width
                //% "AI Score:"
                label: qsTrId("chum-pkg-ai-code")
                value: pkg.aiCode + " - " + infoItem.aiScore(pkg.aiCode)
                alignment: Qt.AlignLeft
            }
        }
        Label {
            width: parent.width
            text: pkg.aiDescription
            color: infoItem.pressed ? Theme.highlightColor : Theme.secondaryColor
            linkColor: Theme.highlightColor
            visible: !!pkg.aiDescription
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeSmall
            onLinkActivated: Qt.openUrlExternally(link)
        }
        ChumDetailItem {
            visible: !!pkg.aiDescriptionMDUrl
            text: '<font color="%1">%3</font> <font color="%2"><a href="%4">%4</a></font>'
            .arg(Theme.secondaryHighlightColor)
            .arg(Theme.primaryColor)
            //% "AI Usage Details:"
            .arg(qsTrId("chum-pkg-ai-desc-link-label"))
            .arg(pkg.aiDescriptionMDUrl)

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
