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
                //% "Categories"
                title: qsTrId("chum-categories")
            }
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            onClicked: pageStack.push(Qt.resolvedUrl("../pages/PackagesListPage.qml"), {
                                          title: model.category,
                                          applicationsOnly: model.categoryIds === 'Library' ? false : Chum.showAppsByDefault,
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
                //% "Libraries"
                category: qsTrId("chum-category-libraries")
                categoryIds: "Library"
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
            ListElement {
                //% "Other"
                category: qsTrId("chum-category-other")
                categoryIds: "Other"
            }
        }

        VerticalScrollDecorator {}
    }
}
