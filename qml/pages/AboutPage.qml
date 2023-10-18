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
                id: chumGUIicon
                source: Qt.resolvedUrl("../../icons/sailfishos-chum-gui.svg")
                width: Math.min(page.width, page.height) / 3
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // When using the same, theoretically correct, horizontal margin as in
            // SettingsPage.qml (twice Theme.horizontalPageMargin), centering the text
            // horizontally makes Qt render a much larger margin (circa twice the size).
            // Thus halving the margin: width: parent.width - Theme.horizontalPageMargin
            // The long text of the last label emphasises this issue by using much space
            // vertically when rendered with such big horizontal margin(s).
            
            Label {
                id: subTitle
                //% "A graphical client application for the SailfishOS:Chum community repository"
                text: qsTrId("chum-about-store")
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
            }

            Label {
                id: versionInfo
                //% "Version: %1"
                text: qsTrId("chum-about-version").arg(Qt.application.version)
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
            }

            Label {
                id: sourceCode
                //% "Source code repository"
                text: '<a href="https://github.com/sailfishos-chum/sailfishos-chum-gui">' + qsTrId("chum-about-home") + '</a>'
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                id: license
                //% "License: %1"
                text: qsTrId("chum-about-license").arg("MIT")
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
            }

            Label {
                id: issueTracker
                //% "Issue tracker for bug reports, feature suggestions and help requests"
                text: '<a href="https://github.com/sailfishos-chum/sailfishos-chum-gui/issues">' + qsTrId("chum-about-issues") + '</a>'
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }

            // Formatting might look better, when paragraphs (`<p></p>`) are used, instead
            // of two consequtive line-breaks (`<br /><br />`).
            // Reference: https://doc.qt.io/qt-5/richtext-html-subset.html#supported-tags
            // But trying to achive some vertical space atop the text either way is futile:
            // The Qt5 Rich Text engine eliminates any such space, one has to use the Qt 
            // text `anchors` attributes `top:` and `topMargin:` to achieve that.
            Label {
                //% "<p>"
                //% "The SailfishOS:Chum community repository provides a collection of "
                //% "applications, tools and libraries compiled for various hardware "
                //% "architectures and Sailfish&nbsp;OS release versions."
                //% "</p><p>"
                //% "In contrast to the software distribution model of the Jolla Store or "
                //% "OpenRepos, to which binary packages are uploaded by developers, at "
                //% "SailfishOS:Chum software is compiled and packaged into RPMs in a "
                //% "reproducible manner directly from its source code. The source code used "
                //% "for compiling and packaging is submitted by developers to OBS (Open "
                //% "Build Service), which generates multiple RPM files for different "
                //% "combinations of hardware architectures and Sailfish&nbsp;OS release "
                //% "versions."
                //% "</p><p>"
                //% "This scheme ensures that the complete source code of all packages at "
                //% "SailfishOS:Chum is available and inspectable there, and that all "
                //% "packages are generated solely from this source code. Hence all software "
                //% "packages at SailfishOS:Chum are created in a transparent and fully "
                //% "traceable manner."
                //% "</p><p>"
                //% "By collecting software for Sailfish&nbsp;OS in a single automated build "
                //% "system, collaboration between developers through common packaging of "
                //% "shared libraries etc. is fostered, duplication of work for keeping these "
                //% "common packages up-to-date is eliminated, and it becomes much easier to "
                //% "determine which pieces of software exist and which are missing at the "
                //% "Sailfish&nbsp;OS OBS. Additionally this eases tracing multiple and "
                //% "potentially layered dependencies (&quot;dependency chains&quot;) which is crucial "
                //% "for keeping the software supply chains of complex packages up-to-date."
                //% "</p><p>"
                //% "The SailfishOS:Chum repository is located at the Sailfish&nbsp;OS OBS:<br />"
                //% "<a href='https://build.merproject.org/project/show/sailfishos:chum'>build.merproject.org/project/show/sailfishos:chum</a>"
                //% "</p><p>"
                //% "For the etymological origin and meanings of the word &quot;chum&quot;, see "
                //% "<a href='https://en.wikipedia.org/wiki/Chumming'>en.wikipedia.org:Chumming</a> "
                //% "and <a href='https://en.wiktionary.org/wiki/chum'>en.wiktionary.org:chum</a>.</p>"
                text: qsTrId("chum-about-description")
                textFormat: Text.StyledText
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    top: issueTracker.bottom
                    topMargin: Theme.paddingLarge
                    horizontalCenter: parent.horizontalCenter
                }
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
}
