import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    id: page
    allowedOrientations: Orientation.All
    XmlListModel { id: mainModel
        property string versions: "none"
        source: "https://build.sailfishos.org/public/source/sailfishos:chum/_meta"
        query: "/project/repository"
        XmlRole { query: "@name/string()"; name: "repoName" }
        onStatusChanged: if (status == XmlListModel.Ready) {
            var entries = []
            for (var i =0; i < count; ++i) {
                const e = get(i).repoName.split('_')[0]
                if(entries.indexOf(e) == -1)
                    entries.push(e)
            }
            versions = entries.join(", ")
        }
    }
    XmlListModel { id: testingModel
        property string versions: "none"
        source: "https://build.sailfishos.org/public/source/sailfishos:chum:testing/_meta"
        query: "/project/repository"
        XmlRole { query: "@name/string()"; name: "repoName" }
        onStatusChanged: if (status == XmlListModel.Ready) {
            var entries = []
            for (var i =0; i < count; ++i) {
                const e = get(i).repoName.split('_')[0]
                if(entries.indexOf(e) == -1)
                    entries.push(e)
            }
            versions = entries.join(", ")
        }
    }
    XmlListModel { id: legacyModel
        property string versions: "none"
        source: "https://build.sailfishos.org/public/source/sailfishos:chum:legacy/_meta"
        query: "/project/repository"
        XmlRole { query: "@name/string()"; name: "repoName" }
        onStatusChanged: if (status == XmlListModel.Ready) {
            var entries = []
            for (var i =0; i < count; ++i) {
                const e = get(i).repoName.split('_')[0]
                if(entries.indexOf(e) == -1)
                    entries.push(e)
            }
            versions = entries.join(", ")
        }
    }

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
                                    //% "<html>Subscribed to the SailfishOS:Chum:Testing repository with a manually set Sailfish&nbsp;OS version (%1).</html>"
                                    qsTrId("chum-settings-status-repo-testing-manual").arg(Chum.manualVersion) :
                                    //% "<html>Subscribed to the regular SailfishOS:Chum repository with a manually set Sailfish&nbsp;OS version (%1).</html>"
                                    qsTrId("chum-settings-status-repo-regular-manual").arg(Chum.manualVersion)
                    return Chum.repoTesting ?
                                //% "<html>Subscribed to the SailfishOS:Chum:Testing repository with an automatically determined Sailfish&nbsp;OS version.</html>"
                                qsTrId("chum-settings-status-repo-testing-auto") :
                                //% "<html>Subscribed to the regular SailfishOS:Chum repository with an automatically determined Sailfish&nbsp;OS version.</html>"
                                qsTrId("chum-settings-status-repo-regular-auto");

                }
                wrapMode: Text.WordWrap
            }

            SectionHeader {
                //% "General settings"
                text: qsTrId("chum-settings-general")
            }

            TextSwitch {
                checked: Chum.showAppsByDefault
                //% "When listing available software packages, by default only applications are shown. "
                //% "But every listing can be toggled between showing solely applications or "
                //% "all packages by using the corresponding switch in the pulley menu."
                description: qsTrId("chum-settings-show-apps-description")
                //% "Show applications only"
                text: qsTrId("chum-settings-show-apps")
                onClicked: Chum.showAppsByDefault = !Chum.showAppsByDefault
            }

            SectionHeader {
                //% "Advanced settings"
                text: qsTrId("chum-settings-advanced")
            }

            TextSwitch {
                automaticCheck: false
                busy: Chum.busy
                checked: Chum.repoLegacy
                //% "Add the SailfishOS:Chum:Legacy repository. This provides obsolete or unmaintained packages. "
                //% "Note that only some repos are available. "
                //% "Careful about selecting a version other than the one your device is currently running! "
                //% "The following versions are available: %1"
                description: qsTrId("chum-settings-legacy-description").arg(legacyModel.versions)
                //% "Add legacy repository"
                text: qsTrId("chum-settings-legacy")
                onClicked: Chum.repoLegacy = !Chum.repoLegacy;
            }

            TextSwitch {
                automaticCheck: false
                busy: Chum.busy
                checked: Chum.repoTesting
                //% "Use the SailfishOS:Chum:Testing repository. This is mainly useful for developers "
                //% "to test their packages before publishing."
                description: qsTrId("chum-settings-testing-description")
                //% "Use testing repository"
                text: qsTrId("chum-settings-testing")
                onClicked: Chum.repoTesting = !Chum.repoTesting;
            }

            // TODO/FIXME: allow separate version for legacy repo
            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                //% "Override the automatic SailfishOS:Chum repository selection"
                text: qsTrId("chum-settings-override-selection")
                wrapMode: Text.WordWrap
            }

            // `<big>`, `<kbd>`, `<dfn>`, `<tt>` and `<code>` do not work, although all should,
            // see https://doc.qt.io/qt-5/richtext-html-subset.html#supported-tags
            // Hence using `<b>` (bold) and `<i>` (italic) instead.
            TextFieldDesc {
                id: txtRelease
                //% "<b>Press <kbd>Enter</kbd> to apply</b><br />"
                //% "Usually the correct SailfishOS:Chum repository is automatically selected according to the "
                //% "installed Sailfish&nbsp;OS release. To manually select a repository "
                //% "for a specific release, specify this release here (for example, <i>5.1</i>). "
                //% "This is useful when no repository is available for the installed "
                //% "Sailfish&nbsp;OS version, as for cBeta users.<br />"
                //% "For releases prior to 4.6, the complete version number must be provided (for example, <i>4.3.0.12</i>)."
                //% "<br />The following versions are known:<br /> %1"
                description: qsTrId("chum-setings-override-release-description").arg( Chum.repoTesting ? testingModel.versions : mainModel.versions)
                //% "<i>Specify a Sailfish&nbsp;OS version</i>"
                placeholderText: qsTrId("chum-setings-override-release-placeholder")
                text: Chum.manualVersion
                acceptableInput: Chum.repoTesting
                                 ? (testingModel.versions.indexOf(text) != -1)
                                 : (mainModel.versions.indexOf(text) != -1)
                onEnterClicked: {
                    console.log("Setting release to ", txtRelease.text);
                    Chum.manualVersion = txtRelease.text;
                }
            }
        }
    }
}
