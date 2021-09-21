import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
  property alias pkid: pkg.pkid
  property alias title: header.title
  property alias version: availableVersionItem.value

  id: page
  allowedOrientations: Orientation.All

  ChumPackage {
    id: pkg
  }

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: content.height

    PullDownMenu {
      MenuItem {
        //% "Install"
        text: qsTrId("chum-install")
        onClicked: pkg.install()
      }
    }

    Column {
      id: content
      width: parent.width
      spacing: Theme.paddingLarge

      PageHeader {
        id: header
        description: pkg.summary
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
          visible: value
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
    }
  }
}
