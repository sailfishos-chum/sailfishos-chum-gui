import QtQuick 2.0
import Sailfish.Silica 1.0
import org.chum 1.0
import "."

Item {
    id: item

    property ChumPackage pkg

    anchors {
      left: parent.left
      leftMargin: Theme.horizontalPageMargin
      right: parent.right
      rightMargin: Theme.horizontalPageMargin
    }
    height: Math.max(info.height, stickers.height)

    Column {
        id: info
        anchors {
            left: item.left
            right: stickers.left
            rightMargin: Theme.paddingLarge
            verticalCenter: item.verticalCenter
        }

        ImageLabel {
            image: pkg.updateAvailable ?
                       "image://theme/icon-s-update" :
                       "image://theme/icon-s-installed"
            label: {
                if (pkg.updateAvailable)
                    //% "Update available"
                    return qsTrId("chum-pkg-update-available");
                if (pkg.installed)
                    //% "Installed"
                    return qsTrId("chum-pkg-installed");
                return "";
            }
            visible: label
            width: parent.width
        }

        ChumDetailItem {
          id: categories
          //% "Categories"
          label: qsTrId("chum-pkg-categories")
          value: pkg.categories.join(" ")
          visible: value
        }
    }

    Row {
        id: stickers
        anchors {
          right: parent.right
          rightMargin: Theme.horizontalPageMargin
          verticalCenter: item.verticalCenter
        }
        spacing: Theme.paddingLarge

        ImageLabel {
            image: "image://theme/icon-s-favorite"
            label: pkg.starsCount
            visible: pkg.starsCount >= 0
        }

        ImageLabel {
            image: "image://theme/icon-s-developer"
            label: pkg.forksCount
            visible: pkg.forksCount >= 0
        }
    }
}
