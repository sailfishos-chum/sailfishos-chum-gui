import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge

        Column {
            id: content
            spacing: Theme.paddingLarge
            width: parent.width

            PageHeader {
                //% "About Chum GUI"
                title: qsTrId("chum-about-gui")
            }

            Image {
                source: Qt.resolvedUrl("../../icons/sailfishos-chum-gui.svg")
                width: parent.width / 3
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                //% "A store frontend for the Chum repository"
                text: qsTrId("chum-about-store")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                //% "Version: %1"
                text: qsTrId("chum-about-version").arg(Qt.application.version)
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                //% "License: %1"
                text: qsTrId("chum-about-license").arg("MIT")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                //% "Home"
                text: '<a href="https://github.com/sailfishos-chum/sailfishos-chum-gui">' + qsTrId("chum-about-home") + '</a>'
                anchors.horizontalCenter: parent.horizontalCenter
                linkColor: Theme.highlightColor
                wrapMode: Text.WordWrap
                onLinkActivated: openLink(link)
            }

            Label {
                //% "Issues"
                text: '<a href="https://github.com/sailfishos-chum/sailfishos-chum-gui/issues">' + qsTrId("chum-about-issues") + '</a>'
                anchors.horizontalCenter: parent.horizontalCenter
                linkColor: Theme.highlightColor
                wrapMode: Text.WordWrap
                onLinkActivated: openLink(link)
            }

            Label {
                //% "Sailfish OS Community repositories provide a collection of applications, tools, "
                //% "and libraries compiled for different combinations of architectures and Sailfish versions."
                //% ""
                //% "The ambition is to become the main repository for software distribution on Sailfish OS.
                //% "When compared to software distribution via Jolla Store or OpenRepos, the software "
                //% "is compiled into RPMs in a reproducible way directly from the source. The source used "
                //% "for the compilation is available at OBS together with the compiled packages. This is in "
                //% "contrast with the Jolla Store and OpenRepos where all packages are uploaded in binary form "
                //% "without any control over how the binary was compiled."
                //% ""
                //% "By collecting the software in a single automated build system, we can benefit from "
                //% "collaboration between developers through shared packaging of required libraries, reduce "
                //% "duplication of work by keeping the packages up to date, and get a clear overview of "
                //% "missing software."
                text: qsTrId("chum-about-description")
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
