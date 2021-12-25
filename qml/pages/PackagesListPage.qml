import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0

Page {
  property string subTitle
  property string search
  property alias  applicationsOnly: chumModel.filterApplicationsOnly
  property alias  installedOnly: chumModel.filterInstalledOnly
  property alias  updatesOnly: chumModel.filterUpdatesOnly

  id: page
  allowedOrientations: Orientation.All

  SilicaListView {
    id: view
    anchors.fill: parent

    header: Column {
        spacing: Theme.paddingLarge
        width: view.width

        PageHeader {
            title: "Chum"
            description: subTitle
        }

        SearchField {
            id: searchField
            text: page.search
            width: parent.width
            //% "Search"
            placeholderText: qsTrId("chum-search")
            onTextChanged: page.search = text
        }
    }

    // prevent newly added list delegates from stealing focus away from the search field
    currentIndex: -1

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
      search: page.search
    }

    PullDownMenu {
      busy: Chum.busy
      MenuItem {
        text: page.applicationsOnly ?
              //% "Show all packages"
              qsTrId("chum-packages-list-show-all") :
              //% "Show applications only"
              qsTrId("chum-packages-list-show-apps")
        onClicked: page.applicationsOnly = !page.applicationsOnly
      }
    }

    VerticalScrollDecorator {}
  }
}
