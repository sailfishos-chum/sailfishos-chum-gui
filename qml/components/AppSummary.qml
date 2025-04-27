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
        spacing: Theme.paddingSmall

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

        ImageLabel {
            image: {
                if (pkg.type === ChumPackage.PackageApplicationDesktop)
                    return "image://theme/icon-s-sailfish"
                if (pkg.type === ChumPackage.PackageApplicationConsole)
                    return "../../icons/icon-s-consoleapplication.svg"
                return "image://theme/icon-s-clipboard"; // other types
            }
            label: {
                if (pkg.type === ChumPackage.PackageApplicationDesktop)
                    //% "Sailfish application"
                    return qsTrId("chum-pkg-type-desktop");
                if (pkg.type === ChumPackage.PackageApplicationConsole)
                    //% "Console application"
                    return qsTrId("chum-pkg-type-console");
                return ""; // will hide other types
            }
            visible: label
            width: parent.width
        }

        ChumDetailItem {
            id: categories
            //% "Categories:"
            label: qsTrId("chum-pkg-categories")
            value: pkg.categories.join(", ")
            visible: value
        }
    }

    Row {
        id: stickers
        anchors {
            right: parent.right
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
