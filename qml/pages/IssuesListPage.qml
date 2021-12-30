import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    property ChumPackage pkg
    property LoadableObject issues

    property ListModel _model: ListModel {}

    id: page
    allowedOrientations: Orientation.All

    BusyIndicator {
        id: busyInd
        anchors.centerIn: parent
        running: !issues.ready
        size: BusyIndicatorSize.Large
    }

    SilicaListView {
        anchors.fill: parent

        header: PageHeader {
            title: pkg.name
            //% "Issues"
            description: qsTrId("chum-issues")
        }

        model: _model
        delegate: ListItem {
            id: litem
            contentHeight: content.height + Theme.paddingLarge

            onClicked: pageStack.push(Qt.resolvedUrl("../pages/IssuePage.qml"), {
                                          pkg: pkg,
                                          issue: pkg.issue(model.id)
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
                    text: model.title
                    wrapMode: Text.WordWrap
                    color: litem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }

                Item {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin
                    }
                    height: Math.max(authLab.implicitHeight, countImL.implicitHeight)

                    Label {
                        id: authLab
                        anchors {
                            left: parent.left
                            right: countImL.visible ? countImL.left : parent.right
                            rightMargin: countImL.visible ? Theme.paddingLarge : 0
                            verticalCenter: parent.verticalCenter
                        }
                        font.pixelSize: Theme.fontSizeSmall
                        //% "#%1 by %2"
                        text: qsTrId("chum-issue-nr-by").arg(model.number).arg(model.author)
                        truncationMode: TruncationMode.Fade
                        color: litem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    }

                    ImageLabel {
                        id: countImL
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        image: "image://theme/icon-s-chat"
                        label: model.commentsCount
                        textColor: litem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        visible: model.commentsCount > 0
                    }
                }


                Label {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin
                    }
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: {
                        if (model.created === model.updated)
                            return model.created;
                        return qsTrId("chum-created-updated-datetime").
                               arg(model.created).arg(model.updated);
                    }
                    truncationMode: TruncationMode.Fade
                    color: litem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }
            }
        }


    }

    Connections {
        target: issues
        onValueChanged: update()
    }

    Component.onCompleted: update()

    function update() {
        _model.clear();
        if (issues.value && issues.value["issues"])
            issues.value["issues"].forEach(function (e) {
                _model.append(e);
            });
    }
}
