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
                //% "About SailfishOS:Chum GUI"
                title: qsTrId("chum-about-gui")
            }

            Image {
                source: Qt.resolvedUrl("../../icons/sailfishos-chum-gui.svg")
                width: Math.min(page.width, page.height) / 3
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                //% "A graphical application for the SailfishOS:Chum community repository"
                text: qsTrId("chum-about-store")
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
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
                //% "Source code"
                text: '<a href="https://github.com/sailfishos-chum/sailfishos-chum-gui">' + qsTrId("chum-about-home") + '</a>'
                anchors.horizontalCenter: parent.horizontalCenter
                linkColor: Theme.highlightColor
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                //% "Issue tracker for bug reports, feature suggestions and help requests"
                text: '<a href="https://github.com/sailfishos-chum/sailfishos-chum-gui/issues">' + qsTrId("chum-about-issues") + '</a>'
                anchors.horizontalCenter: parent.horizontalCenter
                linkColor: Theme.highlightColor
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {           
                //% "The SailfishOS:Chum community repository provides a collection of "
                //% "applications, tools and libraries compiled for various hardware "
                //% "architectures and Sailfish&nbsp;OS release versions.<br />"
                //% "<br />"
                //% "In contrast to the software distribution model of the Jolla Store or "
                //% "OpenRepos, to which binary packages are uploaded by developers, at "
                //% "SailfishOS:Chum software is compiled and packaged into RPMs in a "
                //% "reproducible manner directly from its source code. The source code used "
                //% "for compiling and packaging is submitted by developers to OBS (Open "
                //% "Build Service), which generates multiple RPM files for different "
                //% "combinations of hardware architectures and Sailfish&nbsp;OS release "
                //% "versions.<br />"
                //% "<br />"
                //% "This scheme ensures that the complete source code of all packages at "
                //% "SailfishOS:Chum is available and inspectable there, and that all "
                //% "packages are generated solely from this source code. Hence all software "
                //% "packages at SailfishOS:Chum are created in a transparent and fully "
                //% "traceable manner.<br />"
                //% "<br />"
                //% "By collecting software for Sailfish&nbsp;OS in a single automated build "
                //% "system, collaboration between developers through common packaging of "
                //% "shared libraries etc. is fostered, duplication of work for keeping these "
                //% "common packages up-to-date is eliminated, and it becomes much easier to "
                //% "determine which pieces of software exist and which are missing at the "
                //% "Sailfish&nbsp;OS OBS. Additionally this eases tracing multiple and "
                //% "potentially layered dependencies (&quot;dependency chains&quot;) which is crucial "
                //% "for keeping the software supply chains of complex packages up-to-date.<br />"
                //% "<br />"
                //% "The SailfishOS:Chum repository is located at the Sailfish&nbsp;OS OBS:<br />"
                //% "<a href='https://build.merproject.org/project/show/sailfishos:chum'>https://build.merproject.org/project/show/sailfishos:chum</a><br />"
                //% "<br />"
                //% "For the etymological origin and meanings of the word &quot;chum&quot;, see "
                //% "<a href='https://en.wikipedia.org/wiki/Chumming'>en.wikipedia.org:Chumming</a> "
                //% "and <a href='https://en.wiktionary.org/wiki/chum'>en.wiktionary.org:chum</a>."
                text: qsTrId("chum-about-description")
                textFormat: Text.StyledText
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignHCenter
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
}
