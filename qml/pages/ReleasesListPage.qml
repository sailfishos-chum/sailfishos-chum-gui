import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    property ChumPackage pkg
    property LoadableObject releases

    property ListModel _model: ListModel {}

    id: page
    allowedOrientations: Orientation.All

    BusyIndicator {
        id: busyInd
        anchors.centerIn: parent
        running: !releases.ready
        size: BusyIndicatorSize.Large
    }

    SilicaListView {
        anchors.fill: parent

        header: PageHeader {
            title: pkg.name
            //% "Releases"
            description: qsTrId("chum-releases")
        }

        model: _model
        delegate: ListItem {
            id: litem
            contentHeight: content.height + Theme.paddingLarge

            onClicked: pageStack.push(Qt.resolvedUrl("../pages/ReleasePage.qml"), {
                                          pkg: pkg,
                                          release: pkg.release(model.id)
                                      })

            Column {
                id: content
                anchors.verticalCenter: parent.verticalCenter
                spacing: Theme.paddingSmall
                width: parent.width

                Label {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin
                    }
                    text: model.name
                    wrapMode: Text.WordWrap
                    color: litem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                Label {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin
                    }
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignRight
                    text: model.datetime
                    wrapMode: Text.WordWrap
                    color: litem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }
            }
        }


    }

    Connections {
        target: releases
        onValueChanged: update()
    }

    Component.onCompleted: update()

    function update() {
        _model.clear();
        if (releases.value && releases.value["releases"])
            releases.value["releases"].forEach(function (e) {
                _model.append(e);
            });
    }
}
