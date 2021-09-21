import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

Page {
  id: page
  allowedOrientations: Orientation.All

  ChumPackageModel {
    id: packageModel
  }

  SilicaListView {
    anchors.fill: parent
    model: packageModel

    header: PageHeader {
      title: "Chum"
      //% "Available packages"
      description: qsTrId("chum-available-packages")
    }

    delegate: ListItem {
      height: Theme.itemSizeMedium

      onClicked: pageStack.push(Qt.resolvedUrl("PackagePage.qml"), {
        pkid:    model.packageId,
        title:   model.packageName,
        version: model.packageVersion
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

    PullDownMenu {
      MenuItem {
        //% "Reload"
        text: qsTrId("chum-reload")
        onClicked: packageModel.reset()
      }
    }

    VerticalScrollDecorator {}
  }
}
