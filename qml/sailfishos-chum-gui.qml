import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Configuration 1.0
import Nemo.Notifications 1.0
import org.chum 1.0
import "pages"

ApplicationWindow {
  property int _updatesCount: 0
  readonly property string chumDisplayName: "Chum"
  readonly property string configGroup:     "/apps/sailfishos-chum-gui/"

  initialPage: Component { MainPage { } }
  cover: Qt.resolvedUrl("cover/CoverPage.qml")
  allowedOrientations: defaultAllowedOrientations

  ConfigurationValue {
    id: updatesNotificationId
    key: configGroup + "updates-notification-id"
    defaultValue: 0
  }

  Notification {
    function showPopup(title, message, icn) {
      replacesId = 0
      previewSummary = title
      previewBody = message
      icon = icn || Qt.application.name
      publish()
    }

    function show(message, icn) {
      showPopup("", message, icn)
    }

    id: notification
    appName: chumDisplayName
    expireTimeout: 3000
  }

  Notification {
    id: updatesNotification
    appName: chumDisplayName
    appIcon: Qt.application.name
    //% "%n update(s) available"
    summary: qsTrId("chum-updates-available", _updatesCount)
  }

  Chum {
    id: chum

    onRefreshingRepoChanged: {
      if (!refreshingRepo) {
        //% "Repository refreshed"
        notification.show(qsTrId("chum-repo-refreshed"))
      }
    }

    onUpdatesCountChanged: {
      if (_updatesCount !== updatesCount) {
        _updatesCount = updatesCount
        if (_updatesCount > 0) {
          updatesNotification.replacesId = updatesNotificationId.value
          updatesNotification.publish()
          updatesNotificationId.value = updatesNotification.replacesId
        } else {
          updatesNotification.close()
          updatesNotificationId.value = 0
        }
      }
    }

    Component.onCompleted: getUpdates()
  }
}
