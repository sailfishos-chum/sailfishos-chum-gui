import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
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
            //% "Categories"
            description: qsTrId("chum-categories")
        }
    }

    delegate: ListItem {
      contentHeight: Theme.itemSizeSmall
      onClicked: pageStack.push(Qt.resolvedUrl("../pages/PackagesListPage.qml"), {
                                    //% "%1 (packages)"
                                    subTitleAll: qsTrId("chum-available-packages-category").arg(model.category),
                                    //% "%1 (applications)"
                                    subTitleApp: qsTrId("chum-available-apps-category").arg(model.category),
                                    applicationsOnly: Chum.showAppsByDefault,
                                    category: model.categoryIds
      })

      Label {
        anchors {
          left: parent.left
          leftMargin: Theme.horizontalPageMargin
          right: parent.right
          rightMargin: Theme.horizontalPageMargin
          verticalCenter: parent.verticalCenter
        }
        text: model.category
        wrapMode: Text.WordWrap
        color: parent.highlighted ? Theme.highlightColor : Theme.primaryColor
      }
    }

    model: ListModel {
        ListElement {
            //% "Accessibility"
            category: qsTrId("chum-category-accessibility")
            categoryIds: "Accessibility"
        }
        ListElement {
            //% "Development"
            category: qsTrId("chum-category-development")
            categoryIds: "Development"
        }
        ListElement {
            //% "Education"
            category: qsTrId("chum-category-education")
            categoryIds: "Education"
        }
        ListElement {
            //% "Games"
            category: qsTrId("chum-category-games")
            categoryIds: "Game"
        }
        ListElement {
            //% "Graphics"
            category: qsTrId("chum-category-graphics")
            categoryIds: "Graphics"
        }
        ListElement {
            //% "Location and Navigation"
            category: qsTrId("chum-category-maps")
            categoryIds: "Maps"
        }
        ListElement {
            //% "Multimedia"
            category: qsTrId("chum-category-multimedia")
            categoryIds: "AudioVideo;Audio;Video"
        }
        ListElement {
            //% "Network"
            category: qsTrId("chum-category-network")
            categoryIds: "Network"
        }
        ListElement {
            //% "Office"
            category: qsTrId("chum-category-office")
            categoryIds: "Office"
        }
        ListElement {
            //% "Science"
            category: qsTrId("chum-category-science")
            categoryIds: "Science"
        }
        ListElement {
            //% "Utilities"
            category: qsTrId("chum-category-utilities")
            categoryIds: "System;Utility"
        }
    }

    VerticalScrollDecorator {}
  }
}
