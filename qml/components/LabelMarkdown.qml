import QtQuick 2.0
import Sailfish.Silica 1.0

import ".."

Label {
    property bool fetching: false
    property var  url

    textFormat: Text.RichText
    Component.onCompleted: fetch()
    onUrlChanged: fetch()

    function fetch() {
        if (!url) {
            text = "";
            return;
        }

        fetching = true;
        //% "Loading..."
        text = qsTrId("chum-loading-text");
        var xhr = new XMLHttpRequest;
        xhr.open("GET", url);
        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.responseText === "") text = "";
                else text = MarkdownParser.parse(xhr.responseText);
            }
        }
        xhr.send()
    }
}
