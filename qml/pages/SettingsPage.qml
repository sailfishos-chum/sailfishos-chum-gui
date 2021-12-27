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
                        //% "Chum repository management failed"
                        return qsTrId("chum-settings-status-repo-management-failed");
                    }
                    return Chum.repoAvailable ?
                          //% "Current status: Chum repository available"
                          qsTrId("chum-settings-status-repo-available") :
                          //% "Current status: Chum repository is not available"
                          qsTrId("chum-settings-status-repo-not-available");
                }
                wrapMode: Text.WordWrap
            }

            TextSwitch {
                automaticCheck: false
                busy: Chum.busy
                checked: Chum.repoTesting
                //% "Use testing version of Chum repository. This is mainly useful for developers "
                //% "for testing their packages before publishing."
                description: qsTrId("chum-settings-testing-description")
                //% "Use testing repository"
                text: qsTrId("chum-settings-testing")

                onClicked: Chum.repoTesting = !Chum.repoTesting;
            }
        }
    }
}
