// As older SFOS doesn't have description property in TextField, use
// this item to have text field with the description

import QtQuick 2.0
import Sailfish.Silica 1.0

Column {
    property alias description: desc.text
    property alias text: entry.text
    property alias placeholderText: entry.placeholderText

    signal enterClicked

    anchors {
        left: parent.left
        right: parent.right
    }

    TextField {
        id: entry
        labelVisible: false
        width: parent.width
        EnterKey.enabled: true
        EnterKey.onClicked: {
            enterClicked();
            focus = false;
        }
    }

    Label {
        id: desc
        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        height: text ? implicitHeight : 0
        visible: text
        wrapMode: Text.WordWrap
    }
}
