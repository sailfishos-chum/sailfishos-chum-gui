import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
  property ChumPackage pkg

  id: page
  allowedOrientations: Orientation.All

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: content.height + Theme.paddingLarge

    PullDownMenu {
      busy: Chum.busy
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
        text: pkg.installed
          //% "Update"
          ? qsTrId("chum-update")
          //% "Install"
          : qsTrId("chum-install")
        visible: !pkg.installed || pkg.updateAvailable
        onClicked: pkg.installed
          ? Chum.updatePackage(pkg.id)
          : Chum.installPackage(pkg.id)
      }
      MenuItem {
        //% "Uninstall"
        text: qsTrId("chum-uninstall")
        visible: pkg.installed
        onClicked: Chum.uninstallPackage(pkg.id)
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
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.rightMargin: Theme.horizontalPageMargin
      }

      AppInformation {
        pkg: page.pkg
        shrunkHeight: page.height/4
        enableExpansion: screenshots.visible || btnDonate.visible || btnReleases.visible || btnIssues.visible
      }

      ScreenshotsBox {
        id: screenshots
        screenshots: pkg.screenshots
      }

      Column {
        width: parent.width

        MoreButton {
          id: btnReleases
          visible: pkg.releasesCount > 0
          //% "Releases (%1)"
          text: qsTrId("chum-releases-number").arg(pkg.releasesCount)
          onClicked: pageStack.push(Qt.resolvedUrl("../pages/ReleasesListPage.qml"), {
                                        pkg: pkg,
                                        releases: pkg.releases()
                                    })
        }

        MoreButton {
          id: btnIssues
          visible: pkg.issuesCount > 0
          //% "Issues (%1)"
          text: qsTrId("chum-issues-number").arg(pkg.issuesCount)
          onClicked: pageStack.push(Qt.resolvedUrl("../pages/IssuesListPage.qml"), {
                                        pkg: pkg,
                                        issues: pkg.issues()
                                    })
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
