import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.lipstick 0.1 as Lipstick
import org.chum 1.0
import "../components"

Page {
    property ChumPackage pkg

    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge

        PullDownMenu {
            busy: Chum.busy
            MenuItem {
                enabled: !Chum.busy
                //% "Remove"
                text: qsTrId("chum-uninstall")
                visible: pkg.installed
                //% "Removing"
                onClicked: remorse.execute(qsTrId("chum-uninstalling"),
                                           function() { Chum.uninstallPackage(pkg.id) } )
            }
            MenuItem {
                //% "Source code"
                text: qsTrId("chum-package-project-repo")
                onClicked: Qt.openUrlExternally(pkg.repo)
                visible: pkg.repo
            }
            MenuItem {
                //% "Issue tracker"
                text: qsTrId("chum-package-file-issue")
                onClicked: Qt.openUrlExternally(pkg.urlIssues)
                visible: pkg.urlIssues
            }
            MenuItem {
                //% "Discussion forum"
                text: qsTrId("chum-package-discussion-forum")
                onClicked: Qt.openUrlExternally(pkg.urlForum)
                visible: pkg.urlForum
            }
            MenuItem {
                enabled: !Chum.busy
                text: pkg.installed
                //% "Update"
                      ? qsTrId("chum-update")
                        //% "Install"
                      : qsTrId("chum-install")
                visible: !pkg.installed || pkg.updateAvailable
                onClicked: pkg.installed
                           ? Chum.updatePackage(pkg.id)
                           : Chum.installPackage(pkg.id)
            }
            MenuItem {
                //% "Launch"
                text: qsTrId("chum-launch")
                visible: pkg.installed && pkg.type == ChumPackage.PackageApplicationDesktop && pkg.desktopFile.length > 0
                onClicked: {
                    launcher.filePath = pkg.desktopFile
                    console.debug("Application info:" ,JSON.stringify(launcher,null,2))
                    if (launcher.isValid && !launcher.isLaunching && !launcher.isUpdating) {
                        launcher.launchApplication()
                    }
                }
                Lipstick.LauncherItem{ id: launcher }
            }
        }

        RemorsePopup { id: remorse }

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            FancyPageHeader {
                id: header
                title: pkg.name
                author: pkg.packager ? pkg.packager : pkg.developer
                description: pkg.summary
                iconSource: pkg.icon
                packagerShown: pkg.packager
            }

            AppSummary {
                pkg: page.pkg
            }

            AppInformation {
                pkg: page.pkg
                developerShown: header.packagerShown
                shrunkHeight: Math.max(page.height/4, page.height - (y - content.y + content.spacing +
                                                                     (screenshots.visible ? (screenshots.height+content.spacing) : 0) +
                                                                     (btnReleases.visible ? btnReleases.implicitHeight : 0) +
                                                                     (btnIssues.visible ? btnIssues.implicitHeight : 0) +
                                                                     (btnReleases.visible || btnIssues.visible ? content.spacing : 0) +
                                                                     (btnDonate.visible ? (btnDonate.height+content.spacing) : 0)))
                enableExpansion: screenshots.visible || btnDonate.visible || btnReleases.visible || btnIssues.visible
            }

            ScreenshotsBox {
                id: screenshots
                screenshots: pkg.screenshots
            }

            Column {
                width: parent.width

                MoreButton {
                    id: btnReleases
                    visible: pkg.releasesCount > 0
                    //% "Releases (%1)"
                    text: qsTrId("chum-releases-number").arg(pkg.releasesCount)
                    onClicked: pageStack.push(Qt.resolvedUrl("../pages/ReleasesListPage.qml"), {
                                                  pkg: pkg,
                                                  releases: pkg.releases()
                                              })
                }

                MoreButton {
                    id: btnIssues
                    visible: pkg.issuesCount > 0
                    //% "Issues (%1)"
                    text: qsTrId("chum-issues-number").arg(pkg.issuesCount)
                    onClicked: pageStack.push(Qt.resolvedUrl("../pages/IssuesListPage.qml"), {
                                                  pkg: pkg,
                                                  issues: pkg.issues()
                                              })
                }
            }

            Button {
                id: btnDonate
                anchors.horizontalCenter: parent.horizontalCenter
                //% "Donate"
                text: qsTrId("chum-package-donation")
                visible: pkg.donation
                onClicked: {
                    Qt.openUrlExternally(pkg.donation)
                }
            }
        }
    }
}
