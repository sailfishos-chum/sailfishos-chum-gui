#include "chum.h"
#include "chumpackage.h"
#include "chumpackagesmodel.h"
#include "loadableobject.h"
#include "main.h"
#include <sailfishapp.h>

#include <QtQuick>
#include <QNetworkDiskCache>

#define CHUM_REGISTER_TYPE(NAME) \
    qmlRegisterType<NAME>("org.chum", 1, 0, #NAME)

QNetworkAccessManager *nMng{nullptr};
static const quint64 cacheSize = 20 * 1024 * 1024; // 20MB

int main(int argc, char *argv[]) {
    CHUM_REGISTER_TYPE(ChumPackage);
    CHUM_REGISTER_TYPE(ChumPackagesModel);
    CHUM_REGISTER_TYPE(LoadableObject);

    qmlRegisterSingletonType<Chum>("org.chum", 1, 0, "Chum", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return static_cast<QObject *>(Chum::instance());
    });

    SailfishApp::application(argc, argv);
    QCoreApplication::setApplicationVersion(QStringLiteral(CHUMGUI_VERSION));

    nMng = new QNetworkAccessManager(qApp);
    QNetworkDiskCache* nCache = new QNetworkDiskCache(qApp);
    nCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/networkCache/"));
    nCache->setMaximumCacheSize(cacheSize);
    nMng->setCache(nCache);

    QQuickView v;
    v.setSource(SailfishApp::pathToMainQml());
    v.show();

    return QCoreApplication::exec();
}
