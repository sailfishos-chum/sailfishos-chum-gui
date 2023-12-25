#include "chum.h"

#include <PackageKit/Daemon>

#include <QDebug>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>

static char* apiUrl = "http://piggz.co.uk:8081";

using namespace PackageKit;

Chum* Chum::s_instance{nullptr};

static QString s_config_showapps{QStringLiteral("main/showAppsByDefault")};
static QString s_config_manualversion{QStringLiteral("main/manualVersion")};

static inline auto role2operation(Transaction::Role role) {
    switch (role) {
    case Transaction::RoleInstallFiles:
    case Transaction::RoleInstallPackages:
        return Chum::PackageInstallation;
    case Transaction::RoleRemovePackages:
        return Chum::PackageRemove;
    case Transaction::RoleUpdatePackages:
        return Chum::PackageUpdate;
    default:
        return Chum::PackageUnknownOperation;
    }
}

Chum::Chum(QObject *parent)
    : QObject{parent}
{
    connect(&m_ssu, &Ssu::updated, this, &Chum::repositoriesListUpdated);
    connect(&m_ssu, &Ssu::updated, this, &Chum::repoUpdated);
    connect(Daemon::global(), &Daemon::updatesChanged, this, [this]() { this->getUpdates(); });

    QSettings settings;
    m_show_apps_by_default = (settings.value(s_config_showapps, 1).toInt() != 0);
    m_manualVersion = (settings.value(s_config_manualversion, QString()).toString());

    getChumCache();
}

Chum* Chum::instance() {
    if (!s_instance) s_instance = new Chum();
    return s_instance;
}

QString Chum::packageId(const QString &pkg_id)
{
    // Use the name of a package as its package ID to ensure that only a single copy of each package is handled
    return Daemon::packageName(pkg_id);
}

void Chum::setShowAppsByDefault(bool v) {
    if (m_show_apps_by_default == v) return;
    m_show_apps_by_default = v;
    QSettings settings;
    settings.setValue(s_config_showapps, v ? 1 : 0);
    emit showAppsByDefaultChanged();
}

void Chum::setManualVersion(const QString &v) {
    if (!m_ssu.manageRepo()) {
        emit error(qtTrId("chum-repo-management-disabled-title"));
        return;
    }

    if (m_manualVersion == v) return;
    m_manualVersion = v.trimmed();

    QSettings settings;
    settings.setValue(s_config_manualversion, v);
    emit manualVersionChanged();

    m_busy = true;
    emit busyChanged();
    setStatus(qtTrId("chum-add-testing-repo"));
    m_ssu.setRepo(m_manualVersion, m_ssu.repoTesting());
}

/////////////////////////////////////////////////////////////
/// A sequence of methods is used to obtain the metadata of packages.
/// This sequence allows to execute PackageKit calls sequentially
/// in a batch. Nested / overlapping calls seem to not work well 
/// for PackageKit. Note that m_busy is not checked for methods
/// which are only called internally.
///
/// At the end of the update sequence, packagesChanged() is emitted.
///
void Chum::refreshPackages() {
    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }

    m_packages_last_refresh.clear();
    m_packages_last_refresh_installed.clear();

    auto pktr = Daemon::getPackages(Transaction::FilterNotSource);
    //% "Retrieving list of available packages"
    setStatus(qtTrId("chum-get-list-packages"));
    connect(pktr, &Transaction::package,  this, [this](
            [[maybe_unused]] int info,
            const QString &packageID,
            [[maybe_unused]] const QString &summary
            ) {
        QString pn = Daemon::packageName(packageID);
        if (pn.endsWith(QLatin1String("-debuginfo")) ||
                pn.endsWith(QLatin1String("-debugsource")) )
            return;
        QString pd = Daemon::packageData(packageID);
        if (pd == m_ssu.repoName())
            m_packages_last_refresh.insert(packageID);
        else if (pd == QStringLiteral("installed"))
            m_packages_last_refresh_installed.insert(packageID);
    });
    connect(pktr, &Transaction::finished, this, &Chum::refreshPackagesInstalled);
}

void Chum::refreshPackagesInstalled()
{
    // Check which repository provides the installed packages
    QHash<QString,QString> packages;
    for (const auto &p: m_packages_last_refresh_installed)
        packages[p] = Daemon::packageName(p);

    auto tr = Daemon::whatProvides(packages.values());
    connect(tr, &Transaction::package, this, [this](
            [[maybe_unused]] int info,
            const QString &packageID,
            [[maybe_unused]] const QString &summary
            ) {
        QString pd = Daemon::packageData(packageID);
        if (pd == m_ssu.repoName())
            m_packages_last_refresh.insert(packageID);
    });
    connect(tr, &Transaction::finished, this, &Chum::refreshPackagesFinished);
}

void Chum::refreshPackagesFinished()
{
    // Check if some packages are not offered anymore
    QSet<QString> last_ids;
    for (const QString &p: m_packages_last_refresh)
        last_ids.insert(packageId(p));

    // Remove packages from local list, which are not offered anymore
    auto current = m_packages.keys();
    for (const QString &id: current)
        if (!last_ids.contains(id)) {
            ChumPackage *p = m_packages.value(id, nullptr);
            if (p) p->deleteLater();
            m_packages.remove(id);
        }

    // Create or update package entries on the local list
    for (const QString &p: m_packages_last_refresh) {
        const QString id = packageId(p);
        ChumPackage *package = m_packages.value(id, nullptr);
        if (!package) {
            package = new ChumPackage(id, this);
            m_packages[id] = package;
        }
        package->setPkidLatest(p);
    }

    setStatus(QLatin1String(""));
    refreshDetails();
}

void Chum::refreshDetails() {
    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }

    //% "Retrieving the current detail information for installed packages"
    setStatus(qtTrId("chum-get-package-details"));

    QStringList packages;
    for (const ChumPackage *p: m_packages)
        if (p->detailsNeedsUpdate())
            packages.append(p->pkidLatest());

    auto tr = Daemon::getDetails(packages);
    connect(tr, &Transaction::details, this, [this](const auto &v) {
        const QString pkid = v.packageId();
        ChumPackage *p = m_packages.value(this->packageId(pkid), nullptr);
        if (p)
            p->setDetails(v);
        else
            qWarning() << "Found detail infomation of currently unavailable package:" << pkid;
    });

    connect(tr, &Transaction::finished, this, [this]() {
        setStatus(QLatin1String(""));
        this->refreshInstalledVersion();
    });
}

void Chum::refreshInstalledVersion() {
    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }

    //% "Retrieving the currently available versions of installed packages"
    setStatus(qtTrId("chum-get-package-version"));

    QStringList packages;
    for (ChumPackage *p: m_packages) {
        p->clearInstalled();
        packages.append(Daemon::packageName(p->pkidLatest()));
    }

    auto tr = Daemon::resolve(packages, Transaction::FilterInstalled);
    connect(tr, &Transaction::package, this, [this](
            [[maybe_unused]] auto info,
            const auto &packageID,
            [[maybe_unused]] const auto &summary) {
        ChumPackage *p = m_packages.value(this->packageId(packageID), nullptr);
        if (p) p->setPkidInstalled(packageID);
        else
            qWarning() << "Found an installed package, which is currently not available:" << packageID;
    });

    connect(tr, &Transaction::finished, this, [this]() {
        size_t new_count = 0;
        for (ChumPackage *p: m_packages)
            if (p->installed())
                ++new_count;
        if (m_installed_count != new_count) {
            m_installed_count = new_count;
            emit this->installedCountChanged();
        }
        this->setStatus(QLatin1String(""));
        this->getUpdates(true);
    });
}

/// This is the last method in the sequence of methods used to update
/// the metadata of packages. It can be called as a Qt-"slot" for the
/// packagekit signal. The flag `force` is used to distinguish
/// whether the call was internal (i.e., from this class: force=true)
/// or as a signal slot (force=false).
void Chum::getUpdates(bool force) {
    if (m_busy && !force) return;

    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }

    //% "Checking for which installed packages an update is available"
    setStatus(qtTrId("chum-check-updates"));

    for (ChumPackage *p: m_packages)
        p->setUpdateAvailable(false);

    auto pktr = Daemon::getUpdates();
    connect(pktr, &Transaction::package, this, [this](
            [[maybe_unused]] int info,
            const QString &packageID,
            [[maybe_unused]] const QString &summary
            ) {
        ChumPackage *p = m_packages.value(this->packageId(packageID), nullptr);
        if (p) p->setUpdateAvailable(true);
    });
    connect(pktr, &Transaction::finished, this, [this]() {
        size_t new_count = 0;
        for (ChumPackage *p: m_packages)
            if (p->updateAvailable())
                ++new_count;
        if (m_updates_count != new_count) {
            m_updates_count = new_count;
            emit this->updatesCountChanged();
        }
        this->setStatus(QLatin1String(""));
        m_busy = false;
        emit this->busyChanged();
        emit this->packagesChanged();
    });
}

// Refresh repository and update package metadata
void Chum::refreshRepo(bool force) {
    if (m_busy && !force) return;
    if (!m_ssu.repoAvailable()) {
        //% "Failed to refresh SailfishOS:Chum repository, because it is not available!"
        emit error(qtTrId("chum-refresh-repository-impossible"));
        return;
    }

    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }

    //% "Refreshing SailfishOS:Chum repository"
    setStatus(qtTrId("chum-refresh-repository"));

    auto pktr = Daemon::repoSetData(
                m_ssu.repoName(),
                QStringLiteral("refresh-now"),
                QVariant::fromValue(true).toString()
                );
    connect(pktr, &Transaction::finished, this, [this](PackageKit::Transaction::Exit status) {
        setStatus(QLatin1String(""));
        refreshPackages();
        if (status == PackageKit::Transaction::ExitSuccess)
            emit this->repositoryRefreshed();
    });
    connect(pktr, &Transaction::errorCode, this,
            [this](PackageKit::Transaction::Error /*error*/, const QString &details){
        qWarning() << "Failed to refresh Chum repository" << details;
        //% "Failed to refresh SailfishOS:Chum repository!"
        emit error(qtTrId("chum-refresh-repository-failed"));
    });
}

void Chum::repositoriesListUpdated() {
    if (!m_ssu.manageRepo()) {
        // Found SailfishOS:Chum repositories, which were not configured by this app, probably a misconfiguration
        emit errorFatal(
                    //% "Repositories misconfigured"
                    qtTrId("chum-repo-management-disabled-title"),
                    //% "The SailfishOS:Chum GUI application failed to manage the SailfishOS:Chum repository!<br />"
                    //% "You probably have multiple SailfishOS:Chum repositories defined for SSU or disabled a "
                    //% "SailfishOS:Chum repository.<br /><br />"
                    //% "Please remove all references to SailfishOS:Chum repositories by executing this command line as root user:<br />"
                    //% "<b><code>%1</code></b><br />"
                    //% "This SailfishOS:Chum GUI application will add any missing SailfishOS:Chum repository when started again."
                    qtTrId("chum-repo-management-disabled-txt").arg("for i in $(ssu lr | fgrep chum | cut -f 3 -d ' '); do ssu rr $i; done"));
        return;
    } else if (!m_ssu.repoAvailable()) {
        m_busy = true;
        emit busyChanged();
        //% "Adding SailfishOS:Chum repository"
        setStatus(qtTrId("chum-add-repo"));
        m_ssu.setRepo(m_manualVersion);
        return;
    }
    refreshRepo(true);
}

void Chum::setRepoTesting(bool testing) {
    if (!m_ssu.manageRepo()) {
        emit error(qtTrId("chum-repo-management-disabled-title"));
        return;
    }

    if (!m_ssu.repoAvailable() || m_ssu.repoTesting() != testing) {
        m_busy = true;
        emit busyChanged();
        //% "Adding SailfishOS:Chum:Testing repository"
        setStatus(qtTrId("chum-add-testing-repo"));
        m_ssu.setRepo(m_manualVersion, testing);
    }
}

// Operations on packages: Install, remove and update
void Chum::installPackage(const QString &id) {
    if (m_busy) return;
    ChumPackage *p = m_packages.value(id, nullptr);
    if (!p) return; // This package ID does not exist
    m_busy = true;
    emit busyChanged();
    //% "Installing package"
    setStatus(qtTrId("chum-install-package"));
    QString pkid = p->pkidLatest();
    startOperation(Daemon::installPackage(pkid), pkid);
}

void Chum::uninstallPackage(const QString &id) {
    if (m_busy) return;
    ChumPackage *p = m_packages.value(id, nullptr);
    if (!p) return; // This package ID does not exist
    m_busy = true;
    emit busyChanged();
    //% "Removing package"
    setStatus(qtTrId("chum-uninstall-package"));
    QString pkid = p->pkidInstalled();
    startOperation(Daemon::removePackage(pkid), pkid);
}

void Chum::updatePackage(const QString &id) {
    if (m_busy) return;
    ChumPackage *p = m_packages.value(id, nullptr);
    if (!p) return; // This package ID does not exist
    m_busy = true;
    emit busyChanged();
    //% "Updating package"
    setStatus(qtTrId("chum-update-package"));
    QString pkid = p->pkidLatest();
    startOperation(Daemon::updatePackage(pkid), pkid);
}

void Chum::updateAllPackages() {
    if (m_busy) return;
    QStringList pkids;
    for (ChumPackage *p: m_packages)
        if (p->updateAvailable())
            pkids.append(p->pkidLatest());
    if (pkids.isEmpty()) return; // No package(s) to update

    m_busy = true;
    emit busyChanged();
    //% "Updating all packages"
    setStatus(qtTrId("chum-update-all-packages"));
    startOperation(Daemon::updatePackages(pkids), QString{});
}

void Chum::startOperation(Transaction *pktr, const QString &pkg_id) {
    if (!pkg_id.isEmpty())
        connect(pktr, &Transaction::roleChanged, this, [this, pktr, pkg_id]() {
            emit this->packageOperationStarted(
                        role2operation(pktr->role()),
                        Daemon::packageName(pkg_id)
                        );
        });

    connect(pktr, &Transaction::finished, this,
            [this, pktr, pkg_id](PackageKit::Transaction::Exit status, uint /*runtime*/) {
        m_busy = false;
        emit busyChanged();
        setStatus(QLatin1String(""));
        if (status == PackageKit::Transaction::ExitSuccess)
            emit this->packageOperationFinished(
                    role2operation(pktr->role()),
                    Daemon::packageName(pkg_id),
                    Daemon::packageVersion(pkg_id)
                    );
        refreshPackages(); // Update details of all packages and their install-status
    });

    connect(pktr, &Transaction::errorCode, this,
            [this, pktr, pkg_id](PackageKit::Transaction::Error /*error*/, const QString &details){
        qWarning() << "Failed" << role2operation(pktr->role())
                   << pkg_id
                   << details;
        emit error(details);
    });
}

void Chum::setStatus(QString status) {
    if (m_status == status) return;
    m_status = status;
    emit statusChanged();
}

void Chum::getChumCache()
{
    if (!m_busy) {
        m_busy = true;
        emit busyChanged();
    }

    //% "Receiving the chum package cache"
    setStatus(qtTrId("chum-update-cache"));

    QNetworkAccessManager *netManager = new QNetworkAccessManager();

    QObject::connect(netManager,&QNetworkAccessManager::finished,[=](QNetworkReply *reply) {
        if (reply->error()){
            qDebug() << "Unable to get package build times" << reply->errorString();
        }
        else {
            m_chumPackageCache = QJsonDocument::fromJson(reply->readAll());
            qDebug() << "Received chum package cache";
        }
        m_busy = false;
        //% "Loading SailfishOS:Chum repository"
        setStatus(qtTrId("chum-load-repositories"));
        m_ssu.loadRepos();
    });

    // Start the network request
    QNetworkRequest request=QNetworkRequest(QUrl(apiUrl));
    m_busy = true;
    emit busyChanged();
    netManager->get(request);
}

QDateTime Chum::findPackageMTime(const QString &rpm) const
{
    qDebug() << Q_FUNC_INFO << rpm << Chum::instance()->repoName() << Chum::instance()->repoVersion();

    QString repo = Chum::instance()->repoVersion() + "_aarch64"; //TODO need to determine device architecture
    QString projectName = Chum::instance()->repoName().replace("-", ":");
    QString packageName;
    QDateTime mtime = QDateTime::fromMSecsSinceEpoch(0);
    bool found = false;

    QJsonArray projects = m_chumPackageCache.object().value("projects").toArray();

    for (auto project : projects) {
        //qDebug() << project.toObject()["name"];
        if (project.toObject()["name"].toString() == projectName) {
            QJsonArray repos = project.toObject()["repositories"].toArray();
            for (auto repo : repos) {
                //qDebug() << repo.toObject()["name"];
                QJsonArray packages = repo.toObject()["packages"].toArray();
                for (auto package : packages) {
                    //qDebug() << package.toObject()["name"].toString();
                    QJsonArray binaries = package.toObject()["binaries"].toArray();
                    for (auto binary : binaries) {
                        //qDebug() << binary.toString();
                        if (binary.toString().startsWith(rpm)) {
                            packageName = package.toObject()["name"].toString();
                            found = true;
                        }
                    }
                }
            }

            if (found) {
                //loop the source projects
                QJsonArray packages = project.toObject()["packages"].toArray();
                for (auto package : packages) {
                    //qDebug() << package.toObject()["name"];
                    if (package.toObject()["name"].toString() == packageName) {
                        int mt = package.toObject()["mtime"].toString().toInt();
                        mtime = QDateTime::fromMSecsSinceEpoch(1000L * mt);
                        qDebug() << "Found binary " << rpm << " in source package " << packageName << mt << mtime;
                    }
                }
            }
        }
    }
    return mtime;

}
