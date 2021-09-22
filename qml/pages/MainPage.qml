import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
  id: page
  allowedOrientations: Orientation.All

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: content.height

    PullDownMenu {
      MenuItem {
        enabled: false
        //% "About"
        text: qsTrId("chum-about")
      }
    }

    Column {
      id: content
      width: parent.width

      PageHeader {
        title: "Chum"
      }

      MainPageButton {
        enabled: chum.updatesCount > 0
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
