import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
  id: page
  allowedOrientations: Orientation.All

  BusyIndicator {
    id: busyInd
    anchors.centerIn: parent
    running: Chum.busy
    size: BusyIndicatorSize.Large
  }

  Label {
      anchors {
        top: busyInd.bottom
        topMargin: Theme.paddingLarge
        horizontalCenter: parent.horizontalCenter
      }
      color: Theme.highlightColor
      horizontalAlignment: Text.AlignHCenter
      text: Chum.status
      visible: busyInd.running
      width: parent.width - 2*Theme.horizontalPageMargin
      wrapMode: Text.WordWrap
  }

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: content.height

    PullDownMenu {
      busy: Chum.busy

      MenuItem {
        enabled: false
        //% "About"
        text: qsTrId("chum-about")
      }

      MenuItem {
        enabled: !Chum.busy
        //% "Settings"
        text: qsTrId("chum-settings-title")
        onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
      }

      MenuItem {
        enabled: !Chum.busy
        //% "Refresh repository"
        text: qsTrId("chum-refresh-cache")
        onClicked: Chum.refreshRepo()
      }
    }

    Column {
      id: content
      width: parent.width

      PageHeader {
        title: "Chum"
      }

      MainPageButton {
        enabled: Chum.updatesCount > 0
        text: enabled
          ? updatesNotification.summary
          //% "No updates available"
          : qsTrId("chum-no-updates")
        visible: !Chum.busy
        onClicked: pageStack.push(Qt.resolvedUrl("PackagesListPage.qml"), {
                                      //% "Updates"
                                      title: qsTrId("chum-updates"),
                                      updatesOnly: true
                                    })
      }

      MainPageButton {
        //% "Categories"
        text: qsTrId("chum-categories")
        visible: !Chum.busy
        onClicked: pageStack.push(Qt.resolvedUrl("CategoriesPage.qml"))
      }

      MainPageButton {
        text: Chum.showAppsByDefault ?
                  //% "Applications"
                  qsTrId("chum-applications") :
                  //% "Packages"
                  qsTrId("chum-packages")
        visible: !Chum.busy
        onClicked: pageStack.push(Qt.resolvedUrl("PackagesListPage.qml"), {
                                      applicationsOnly: Chum.showAppsByDefault
                                  })
      }

      MainPageButton {
        enabled: Chum.installedCount > 0
        text: enabled
          //% "Installed packages"
          ? qsTrId("chum-installed-packages-no")
          //% "No packages installed"
          : qsTrId("chum-no-installed")
        visible: !Chum.busy
        onClicked: pageStack.push(Qt.resolvedUrl("PackagesListPage.qml"), {
                                      //% "Installed"
                                      title: qsTrId("chum-installed"),
                                      installedOnly: true
                                    })
      }
    }
  }
}
