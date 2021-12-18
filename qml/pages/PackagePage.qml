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
    contentHeight: content.height

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
        anchors {
          left: parent.left
          leftMargin: Theme.horizontalPageMargin
          right: parent.right
          rightMargin: Theme.horizontalPageMargin
        }
        spacing: Theme.paddingMedium

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
}
