import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Configuration 1.0
import Nemo.Notifications 1.0
import org.chum 1.0
import "pages"

ApplicationWindow {
  property int _updatesCount: 0
  readonly property string configGroup: "/apps/sailfishos-chum-gui/"

  initialPage: Component { MainPage { } }
  cover: Qt.resolvedUrl("cover/CoverPage.qml")
  allowedOrientations: defaultAllowedOrientations

  ConfigurationValue {
    id: updatesNotificationId
    key: configGroup + "updates-notification-id"
    defaultValue: 0
  }

  Notification {
    id: updatesNotification
    appName: "Chum"
    appIcon: Qt.application.name
    //% "%n update(s) available"
    summary: qsTrId("chum-updates-available", _updatesCount)
  }

  Chum {
    id: chum

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
