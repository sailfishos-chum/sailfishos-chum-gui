#include "chumpackage.h"
#include "chumpackagemodel.h"

#include <sailfishapp.h>

#include <QtQuick>

int main(int argc, char *argv[]) {
  qmlRegisterType<ChumPackage     >("org.chum", 1, 0, "ChumPackage");
  qmlRegisterType<ChumPackageModel>("org.chum", 1, 0, "ChumPackageModel");

  SailfishApp::application(argc, argv);
  QCoreApplication::setApplicationVersion(QStringLiteral(CHUMGUI_VERSION));

  QQuickView v;
  v.setSource(SailfishApp::pathToMainQml());
  v.show();

  return QCoreApplication::exec();
}
