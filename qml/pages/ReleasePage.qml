import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    property ChumPackage pkg
    property LoadableObject release

    property string releaseDate: release.ready ? release.value.datetime : ""
    property string releaseDescription: release.ready ? release.value.description : ""
    property string releaseName: release.ready ? release.value.name : ""

    id: page
    allowedOrientations: Orientation.All

    BusyIndicator {
        id: busyInd
        anchors.centerIn: parent
        running: !release.ready
        size: BusyIndicatorSize.Large
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge

        Column {
            id: content
            spacing: Theme.paddingLarge
            visible: release.ready
            width: parent.width

            PageHeader {
                title: pkg.name
                //% "Release %1"
                description: qsTrId("chum-release").arg(releaseName)
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                horizontalAlignment: Text.AlignRight
                font.pixelSize: Theme.fontSizeSmall
                text: releaseDate
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                text: releaseDescription
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
}
