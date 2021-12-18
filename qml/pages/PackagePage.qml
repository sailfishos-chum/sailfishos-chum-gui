import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
  property ChumPackage pkg

  readonly property bool _installed: !!pkg.installedVersion

  id: page
  allowedOrientations: Orientation.All

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: content.height + Theme.paddingLarge

    PullDownMenu {
      MenuItem {
        text:qsTr("Project Repository")
        onClicked: Qt.openUrlExternally(pkg.repo)
        visible: pkg.repo
      }
      MenuItem {
        text:qsTr("File Issue")
        onClicked: Qt.openUrlExternally(pkg.urlIssues)
        visible: pkg.urlIssues
      }
      MenuItem {
        text:qsTr("Discussion Forum")
        onClicked: Qt.openUrlExternally(pkg.urlForum)
        visible: pkg.urlForum
      }
      MenuItem {
        text: _installed
          //% "Update"
          ? qsTrId("chum-update")
          //% "Install"
          : qsTrId("chum-install")
        visible: !_installed || pkg.updateAvailable
        onClicked: _installed
          ? Chum.updatePackage(pkid)
          : Chum.installPackage(pkid)
      }
    }

    Column {
      id: content
      width: parent.width
      spacing: Theme.paddingLarge

      FancyPageHeader {
        id: header
        title: pkg.name
        description: {
            if (pkg.summary && pkg.developer)
                return "%1\n%2".arg(pkg.summary).arg(pkg.developer);
            if (pkg.developer) return pkg.developer;
            if (pkg.summary) return pkg.summary;
            return "";
        }
        iconSource: pkg.icon
      }

      Row {
          anchors {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
          }
          spacing: Theme.paddingLarge

          ImageLabel {
              image: "image://theme/icon-m-favorite"
              label: pkg.starsCount
              visible: pkg.starsCount >= 0
          }

          ImageLabel {
              image: "image://theme/icon-m-shuffle"
              label: pkg.forksCount
              visible: pkg.forksCount >= 0
          }
      }

      ChumDetailItem {
        //% "Categories"
        label: qsTrId("chum-pkg-categories")
        value: pkg.categories.join(" ")
        visible: value
      }

      Label {
        anchors {
          left: parent.left
          leftMargin: Theme.horizontalPageMargin
          right: parent.right
          rightMargin: Theme.horizontalPageMargin
        }
        text: pkg.description
        wrapMode: Text.WordWrap
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.highlightColor
      }

      ScreenshotsBox {
        screenshots: pkg.screenshots
      }

      Column {
        spacing: Theme.paddingMedium
        width: parent.width

        ChumDetailItem {
          //% "License"
          label: qsTrId("chum-license")
          value: pkg.license
        }

        ChumDetailItem {
          id: installedVersionItem
          visible: _installed
          //% "Installed version"
          label: qsTrId("chum-pkg-installed-version")
          value: pkg.installedVersion
        }

        ChumDetailItem {
          id: availableVersionItem
          //% "Available version"
          label: qsTrId("chum-pkg-available-version")
        }

        ChumDetailItem {
          //% "Download size"
          label: qsTrId("chum-pkg-download-size")
          value: Format.formatFileSize(pkg.size)
        }

        ChumDetailItem {
          //% "License"
          label: qsTrId("chum-pkg-license")
          value: pkg.license
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

      Column {
        width: parent.width

        MoreButton {
          visible: pkg.releasesCount > 0
          //% "Releases (%1)"
          text: qsTrId("chum-releases-number").arg(pkg.releasesCount)
          onClicked: console.log("Releases list page")
        }

        MoreButton {
          visible: pkg.issuesCount > 0
          //% "Issues (%1)"
          text: qsTrId("chum-issues-number").arg(pkg.issuesCount)
          onClicked: console.log("Issues list page")
        }
      }

      Button {
          id: btnDonate
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("Make Dontation")
          visible: pkg.donation
          onClicked: {
              Qt.openUrlExternally(pkg.donation)
          }
      }
    }
  }
}
