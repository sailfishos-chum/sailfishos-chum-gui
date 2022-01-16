import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge

        Column {
            id: content
            spacing: Theme.paddingLarge
            width: parent.width

            PageHeader {
                //% "Settings"
                title: qsTrId("chum-settings-title")
            }

            SectionHeader {
                //% "Status"
                text: qsTrId("chum-settings-status")
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                text: {
                    if (!Chum.repoManaged) {
                        //% "SailfishOS:Chum repository management failed"
                        return qsTrId("chum-settings-status-repo-management-failed");
                    }
                    if (Chum.busy)
                        return Chum.status

                    if (!Chum.repoAvailable)
                        //% "SailfishOS:Chum repository is not available"
                        return qsTrId("chum-settings-status-repo-not-available");

                    if (Chum.manualVersion)
                        return Chum.repoTesting ?
                                    //% "Subscribed the SailfishOS:Chum testing repository with a manually set Sailfish OS version (%1)"
                                    qsTrId("chum-settings-status-repo-testing-manual").arg(Chum.manualVersion) :
                                    //% "Subscribed the regular SailfishOS:Chum repository with a manually set Sailfish OS version (%1)"
                                    qsTrId("chum-settings-status-repo-regular-manual").arg(Chum.manualVersion)
                    return Chum.repoTesting ?
                                //% "Subscribed the SailfishOS:Chum testing repository with an automatically determined Sailfish OS version"
                                qsTrId("chum-settings-status-repo-testing-auto") :
                                //% "Subscribed the regular SailfishOS:Chum repository with an automatically determined Sailfish OS version"
                                qsTrId("chum-settings-status-repo-regular-auto");

                }
                wrapMode: Text.WordWrap
            }

            SectionHeader {
                //% "General"
                text: qsTrId("chum-settings-general")
            }

            TextSwitch {
                checked: Chum.showAppsByDefault
                //% "When listing available software packages, by default only applications are shown. "
                //% "But for each listing you can switch between showing only applications or "
                //% "all packages by using the corresponding switch in the pulley menu."
                description: qsTrId("chum-settings-show-apps-description")
                //% "Show applications only"
                text: qsTrId("chum-settings-show-apps")
                onClicked: Chum.showAppsByDefault = !Chum.showAppsByDefault
            }

            TextSwitch {
                automaticCheck: false
                busy: Chum.busy
                checked: Chum.repoTesting
                //% "Use the SailfishOS:Chum testing repository. This is mainly useful for developers "
                //% "to test their packages before publishing."
                description: qsTrId("chum-settings-testing-description")
                //% "Use testing repository"
                text: qsTrId("chum-settings-testing")
                onClicked: Chum.repoTesting = !Chum.repoTesting;
            }

            SectionHeader {
                //% "Advanced"
                text: qsTrId("chum-settings-advanced")
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                //% "Override SailfishOS:Chum repository selection"
                text: qsTrId("chum-settings-override-selection")
            }

            TextFieldDesc {
                id: txtRelease
                //% "Usually a specific SailfishOS:Chum repository is automatically selected according to the "
                //% "installed Sailfish OS release version. To subscribe to a SailfishOS:Chum repository for "
                //% "a specific Sailfish OS release, specify this Sailfish OS release here (for example, 4.3.0.12). "
                //% "This is useful when the SailfishOS:Chum repository is not available for the installed "
                //% "Sailfish OS version, as for Cbeta users."
                description: qsTrId("chum-setings-override-release-description")
                //% "Specify Sailfish OS version"
                placeholderText: qsTrId("chum-setings-override-release-placeholder")
                text: Chum.manualVersion
                onEnterClicked: {
                    console.log("Setting release to ", txtRelease.text);
                    Chum.manualVersion = txtRelease.text;
                }
            }
        }
    }
}
