pragma Singleton

import QtQuick 2.0
import Sailfish.Silica 1.0

import ".."

Item {
    property var showdown: ShowDown.showdown
    property var converter: new showdown.Converter({   simplifiedAutoLink: true,
                                                       excludeTrailingPunctuationFromURLs: true,
                                                       strikethrough: true,
                                                       tables: true,
                                                       tasklists: false,
                                                       parseImgDimensions: true,
                                                       simpleLineBreaks: true,
                                                       emoji: true })

    function parse(content) {
        var convertedText = converter.makeHtml(content)

        return "<style>\n" +
                "ul,ol,table,img { margin: " + Theme.paddingLarge + "px 0px; }\n" +
                "img.emoji { height: 1em; width: 1em; margin 0.05em .1em; vertical-align: -0.1em; }\n" +
                "a:link { color: " + Theme.highlightColor + "; }\n" +
                "a.checkbox { text-decoration: none; padding: " + Theme.paddingSmall + "px; display: inline-block; }\n" +
                "li.tasklist { font-size:large; margin: " + Theme.paddingMedium + "px 0px; }\n" +
                "del { text-decoration: line-through; }\n" +
                "table { border-color: " + Theme.secondaryColor + "; }\n" +
                "blockquote { font-style: italic; color: "+ Theme.highlightColor + ";}\n" +
                "h1 { font-size:large; }\n" +
                "h2 { font-size:medium; }\n" +
                "h3 { font-size:medium; }\n" +
                "h4 { font-size:small; }\n" +
                "h5 { font-size:small; }\n" +
                "h6 { font-size:small; }\n" +
                "</style>\n" +
                convertedText
    }
}
