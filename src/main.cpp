#include "chum.h"
#include "chumpackage.h"
#include "chumpackagesmodel.h"
#include "main.h"
#include <sailfishapp.h>

#include <QtQuick>

#define CHUM_REGISTER_TYPE(NAME) \
  qmlRegisterType<NAME>("org.chum", 1, 0, #NAME)

QNetworkAccessManager *nMng{nullptr};

int main(int argc, char *argv[]) {
  CHUM_REGISTER_TYPE(ChumPackage);
  CHUM_REGISTER_TYPE(ChumPackagesModel);

  qmlRegisterSingletonType<Chum>("org.chum", 1, 0, "Chum", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return static_cast<QObject *>(Chum::instance());
    });

  SailfishApp::application(argc, argv);
  QCoreApplication::setApplicationVersion(QStringLiteral(CHUMGUI_VERSION));

  nMng = new QNetworkAccessManager(qApp);

  QQuickView v;
  v.setSource(SailfishApp::pathToMainQml());
  v.show();

  return QCoreApplication::exec();
}
