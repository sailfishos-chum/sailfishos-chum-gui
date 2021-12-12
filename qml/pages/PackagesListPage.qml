import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

Page {
  property string subTitle
  property alias  updatesOnly: chumModel.filterUpdatesOnly

  id: page
  allowedOrientations: Orientation.All

  SilicaListView {
    id: view
    anchors.fill: parent

    header: PageHeader {
      title: "Chum"
      description: subTitle
    }

    delegate: ListItem {
      height: Theme.itemSizeMedium

      onClicked: pageStack.push(Qt.resolvedUrl("../pages/PackagePage.qml"), {
        pkg:    Chum.package(model.packageId)
      })

      Label {
        anchors {
          left: parent.left
          leftMargin: Theme.horizontalPageMargin
          right: parent.right
          rightMargin: Theme.horizontalPageMargin
          verticalCenter: parent.verticalCenter
        }
        text: model.packageName
        wrapMode: Text.WordWrap
        color: parent.highlighted ? Theme.highlightColor : Theme.primaryColor
      }
    }

    model: ChumPackagesModel {
      id: chumModel
    }

    PullDownMenu {
      MenuItem {
        //% "Reload"
        text: qsTrId("chum-reload")
        onClicked: chumModel.reset()
      }
    }

    VerticalScrollDecorator {}
  }
}
