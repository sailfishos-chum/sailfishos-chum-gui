import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
  property alias pkid: pkg.pkid
  property alias title: header.description
  property alias version: availableVersionItem.value
  readonly property bool _installed: !!pkg.installedVersion
  property var meta

  id: page
  allowedOrientations: Orientation.All

  ChumPackage {
    id: pkg

    onUpdated: {
        console.log(pkg.metadata);
        meta = JSON.parse(pkg.metadata);
        console.log(JSON.stringify(meta, null, 4));
        //meta = m;
    }
  }

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: content.height

    PullDownMenu {
      MenuItem {
        text:qsTr("Project Repository")
        onClicked: Qt.openUrlExternally(meta.repo)
        visible: meta.hasOwnProperty("repo")
      }
      MenuItem {
        text:qsTr("File Issue")
        onClicked: Qt.openUrlExternally(meta.issues)
        visible: meta.hasOwnProperty("issues")
      }
      MenuItem {
        text:qsTr("Discussion Forum")
        onClicked: Qt.openUrlExternally(meta.forum)
        visible: meta.hasOwnProperty("forum")
      }
      MenuItem {
        text: _installed
          //% "Update"
          ? qsTrId("chum-update")
          //% "Install"
          : qsTrId("chum-install")
        onClicked: _installed
          ? chum.updatePackage(pkid)
          : chum.installPackage(pkid)
      }
    }

    Column {
      id: content
      width: parent.width
      spacing: Theme.paddingLarge

      FancyPageHeader {
        id: header
        title: pkg.summary
        description: pkid
        iconSource: meta.hasOwnProperty("icon") ? meta.icon : ""
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
        screenshots: meta.hasOwnProperty("screenshots") ? meta.screenshots : []
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
            text: qsTr("Make Dontation")
            visible: meta.hasOwnProperty("donation")
            onClicked: {
                Qt.openUrlExternally(meta.donation)
            }
        }
      }
    }
  }
}
