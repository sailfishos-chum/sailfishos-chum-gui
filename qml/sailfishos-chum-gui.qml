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

  function _packageOperationNotification(operation) {
    switch (operation) {
    case Chum.PackageInstallation:
      //: %1 - package name, %2 - package version
      //% "%1 %2 installed"
      return QT_TRID_NOOP("chum-package-installed")
    case Chum.PackageUpdate:
      //: %1 - package name, %2 - package version
      //% "%1 %2 updated"
      return QT_TRID_NOOP("chum-package-updated")
    default:
      return null
    }
  }

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

  Connections {
    target: Chum

    onError: notification.show(errorTxt)

    onRepositoryRefreshed: {
      //% "Repository refreshed"
      notification.show(qsTrId("chum-repo-refreshed"))
    }

    onUpdatesCountChanged: {
      if (_updatesCount !== Chum.updatesCount) {
        _updatesCount = Chum.updatesCount
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

    onPackageOperationFinished: {
      var trid = _packageOperationNotification(operation)
      if (trid) {
        notification.show(qsTrId(trid).arg(name).arg(version))
      }
    }
  }
}
