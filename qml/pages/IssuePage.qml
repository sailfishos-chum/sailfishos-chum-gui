import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "../components"

Page {
    property ChumPackage pkg
    property LoadableObject issue

    property ListModel _model: ListModel {}
    property int commentsCount: issue.value.commentsCount ? issue.value.commentsCount : 0
    property string number: issue.value.number ? issue.value.number : ""
    property string title: issue.value.title ? issue.value.title : ""

    id: page
    allowedOrientations: Orientation.All

    BusyIndicator {
        id: busyInd
        anchors.centerIn: parent
        running: !issue.ready
        size: BusyIndicatorSize.Large
    }

    SilicaListView {
        anchors.fill: parent

        header: Column {
            width: page.width

            PageHeader {
                title: pkg.name
                description: issue.ready ?
                                 //% "Issue: #%1"
                                 qsTrId("chum-issue-with-number").arg(number) :
                                 //% "Issue"
                                 qsTrId("chum-issue")
            }

            Item {
                height: Theme.paddingMedium
                width: parent.width
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeLarge
                text: page.title
                visible: issue.ready
                wrapMode: Text.WordWrap
            }

            Item {
                height: Theme.paddingSmall
                width: parent.width
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeSmall
                horizontalAlignment: Text.AlignRight
                //% "Comments: %1"
                text: qsTrId("chum-issues-comments-number").arg(page.commentsCount)
                visible: issue.ready
                wrapMode: Text.WordWrap
            }

            Item {
                height: Theme.paddingLarge
                width: parent.width
            }
        }

        model: _model
        delegate: Item {
            id: litem
            height: content.height + Theme.paddingLarge
            width: page.width

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
                    text: model.author
                    wrapMode: Text.WordWrap
                    color: Theme.highlightColor
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
                        //% "Created: %1; Updated: %2"
                        return qsTrId("chum-created-updated-datetime").
                               arg(model.created).arg(model.updated);
                    }
                    truncationMode: TruncationMode.Fade
                    color: Theme.secondaryHighlightColor
                }

                Label {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin
                    }
                    color: Theme.primaryColor
                    text: model.body
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }


    }

    Connections {
        target: issue
        onValueChanged: update()
    }

    Component.onCompleted: update()

    function update() {
        _model.clear();
        if (issue.value && issue.value["discussion"])
            issue.value["discussion"].forEach(function (e) {
                _model.append(e);
            });
    }
}
