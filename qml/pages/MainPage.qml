import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
  id: page
  allowedOrientations: Orientation.All

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
        //% "Refresh cache"
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
        onClicked: pageStack.push(Qt.resolvedUrl("UpdatesPage.qml"))
      }

      MainPageButton {
        text: qsTrId("chum-available-packages")
        onClicked: pageStack.push(Qt.resolvedUrl("AvailablePackagesPage.qml"))
      }
    }
  }
}
