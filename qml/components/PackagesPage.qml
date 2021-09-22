import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

Page {
  property string subTitle
  property alias model: view.model

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
        onClicked: model.reset()
      }
    }

    VerticalScrollDecorator {}
  }
}
