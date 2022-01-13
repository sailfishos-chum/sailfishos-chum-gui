import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    id: page

    property string title
    property string search
    property alias  applicationsOnly: chumModel.filterApplicationsOnly
    property alias  category: chumModel.showCategory
    property alias  installedOnly: chumModel.filterInstalledOnly
    property alias  updatesOnly: chumModel.filterUpdatesOnly

    signal searchFocus;

    allowedOrientations: Orientation.All

    SilicaListView {
        id: view
        anchors.fill: parent

        header: Column {
            spacing: Theme.paddingLarge
            width: view.width

            PageHeader {
                title: page.title ? page.title : _extra
                description: page.title ? _extra : ""
                property string _extra: applicationsOnly ?
                                            qsTrId("chum-applications") :
                                            qsTrId("chum-packages")
            }

            SearchField {
                id: searchField
                text: page.search
                width: parent.width
                //% "Search"
                placeholderText: qsTrId("chum-search")
                onTextChanged: page.search = text
            }

            Connections {
                target: page
                onSearchFocus: {
                    console.log("Signal focus");
                    searchField.forceActiveFocus();
                }
            }
        }

        // prevent newly added list delegates from stealing focus away from the search field
        currentIndex: -1

        delegate: ListItem {
            id: lItem
            contentHeight: item.height

            menu: ContextMenu {
                MenuItem {
                    text: qsTrId("chum-update")
                    onClicked: Chum.updatePackage(model.packageId)
                    visible: model.packageUpdateAvailable
                }
                MenuItem {
                    text: model.packageInstalled ?
                              qsTrId("chum-uninstall") :
                              qsTrId("chum-install")
                    onClicked: model.packageInstalled ?
                                   Chum.uninstallPackage(model.packageId) :
                                   Chum.installPackage(model.packageId)
                }
            }

            onClicked: pageStack.push(Qt.resolvedUrl("../pages/PackagePage.qml"), {
                                          pkg:    Chum.package(model.packageId)
                                      })

            PackagesListItem {
                id: item
                highlighted: lItem.highlighted
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
            MenuItem {
                //% "Update all"
                text: qsTrId("chum-packages-list-apply-all-updates")
                onClicked: Chum.updateAllPackages()
                visible: page.updatesOnly
            }
        }

        VerticalScrollDecorator {}
    }

    Component.onCompleted: {
        if (chumModel.rowCount() > 10) {
            page.searchFocus();
        }
    }
}
