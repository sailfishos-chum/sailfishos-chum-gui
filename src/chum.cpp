#include "chum.h"

#include <PackageKit/Daemon>

#include <QDebug>

using namespace PackageKit;

Chum* Chum::s_instance{nullptr};

#define SET_STATUS(status) { \
  if (m_status != status) {  \
    m_status = status;       \
    emit statusChanged();    \
  }                          \
}

static inline auto role2operation(Transaction::Role role) {
  switch (role) {
  case Transaction::RoleInstallFiles:
  case Transaction::RoleInstallPackages:
    return Chum::PackageInstallation;
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
  connect(Daemon::global(), &Daemon::updatesChanged, [this]() { this->getUpdates(); });

  m_busy = true;
  //% "Load repositories"
  SET_STATUS(qtTrId("chum-load-repositories"));
  m_ssu.loadRepos();
}

Chum* Chum::instance() {
  if (!s_instance) s_instance = new Chum();
  return s_instance;
}

QString Chum::packageId(const QString &pkg_id)
{
  // use the name as package ID to ensure that we have only one copy of each package
  return Daemon::packageName(pkg_id);
}

/////////////////////////////////////////////////////////////
/// Sequence of methods used to fill in data for packages.
/// This sequence allows to call PackageKit calls sequentially
/// in bulk. Netsed calls seem to interfere with the PackageKit.
/// Note that m_busy is not checked for methods that are called
/// internally only.
///
/// At the end of updates, packagesChanged() is emitted.
///
void Chum::refreshPackages() {
  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  m_packages_last_refresh.clear();
  m_packages_last_refresh_installed.clear();

  auto pktr = Daemon::getPackages(Transaction::FilterNotDevel);
  //% "Get list of packages"
  SET_STATUS(qtTrId("chum-get-list-packages"));
  connect(pktr, &Transaction::package,  this, [this](
          [[maybe_unused]] int info,
          const QString &packageID,
          [[maybe_unused]] const QString &summary
          ) {
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
  // check what repository provides installed packages
  QHash<QString,QString> packages;
  for (const auto &p: m_packages_last_refresh_installed)
    packages[p] = Daemon::packageName(p);

  auto tr = Daemon::whatProvides(packages.values());
  connect(tr, &Transaction::package,  this, [this](
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
  // check if some packages disappeared
  QSet<QString> last_ids;
  for (const QString &p: m_packages_last_refresh)
    last_ids.insert(packageId(p));
  auto current = m_packages.keys();
  for (const QString &id: current)
    if (!last_ids.contains(id)) {
      ChumPackage *p = m_packages.value(id, nullptr);
      if (p) p->deleteLater();
      m_packages.remove(id);
    }

  // create or update packages
  for (const QString &p: m_packages_last_refresh) {
    const QString id = packageId(p);
    ChumPackage *package = m_packages.value(id, nullptr);
    if (!package) {
       package = new ChumPackage(id, this);
       m_packages[id] = package;
    }
    package->setPkid(p);
  }

  SET_STATUS(QStringLiteral());
  refreshDetails();
}

void Chum::refreshDetails() {
  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  //% "Get package details"
  SET_STATUS(qtTrId("chum-get-package-details"));

  QStringList packages;
  for (const ChumPackage *p: m_packages.values())
    if (p->detailsNeedsUpdate())
      packages.append(p->pkid());

  auto tr = Daemon::getDetails(packages);
  connect(tr, &Transaction::details, this, [this](const auto &v) {
    const QString pkid = v.packageId();
    ChumPackage *p = m_packages.value(packageId(pkid), nullptr);
    if (p)
      p->setDetails(v);
    else
      qWarning() << "Got details for missing package:" << pkid;
  });

  connect(tr, &Transaction::finished, this, [this]() {
     SET_STATUS(QStringLiteral());
     this->refreshInstalledVersion();
  });
}

void Chum::refreshInstalledVersion() {
  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  //% "Get versions of installed packages"
  SET_STATUS(qtTrId("chum-get-package-version"));

  QStringList packages;
  for (ChumPackage *p: m_packages.values())
    if (p->installedVersionNeedsUpdate()) {
      packages.append(Daemon::packageName(p->pkid()));
      // reset requirement flag that can be set for non-installed package
      p->setInstalledVersion(p->installedVersion());
    }

  static decltype(m_installed_count) new_count;
  new_count = 0;
  auto tr = Daemon::resolve(packages, Transaction::FilterInstalled);
  connect(tr, &Transaction::package, this, [this](
          [[maybe_unused]] auto info,
          const auto &packageID,
          [[maybe_unused]] const auto &summary) {
    ChumPackage *p = m_packages.value(packageId(packageID), nullptr);
    if (p) {
      p->setInstalledVersion(Daemon::packageVersion(packageID));
      ++new_count;
    } else
      qWarning() << "Got installed version for missing package:" << packageID;
  });

  connect(tr, &Transaction::finished, this, [this]() {
      if (m_installed_count != new_count) {
        m_installed_count = new_count;
        emit this->installedCountChanged();
      }
     SET_STATUS(QStringLiteral());
     this->getUpdates(true);
  });
}

/// Last method in the sequence of methods used to update
/// packages information. Can be called as a slot for
/// packagekit signal. So, flag `force` is used to distinguish
/// whether the call was internal (from this class, force=true)
/// or as a signal slot (force=false).
void Chum::getUpdates(bool force) {
  if (m_busy && !force) return;

  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  //% "Check for updates"
  SET_STATUS(qtTrId("chum-check-updates"));

  for (ChumPackage *p: m_packages.values())
    p->setUpdateAvailable(false);

  static decltype(m_updates_count) new_count;
  new_count = 0;
  auto pktr = Daemon::getUpdates();
  connect(pktr, &Transaction::package, this, [this](
    [[maybe_unused]] int info,
    const QString &packageID,
    [[maybe_unused]] const QString &summary
  ) {
      ChumPackage *p = m_packages.value(packageId(packageID), nullptr);
      if (p) {
        p->setUpdateAvailable(true);
        ++new_count;
      }
  });
  connect(pktr, &Transaction::finished, this, [this]() {
    if (m_updates_count != new_count) {
      m_updates_count = new_count;
      emit this->updatesCountChanged();
    }
    SET_STATUS(QStringLiteral());
    m_busy = false;
    emit busyChanged();
    emit packagesChanged();
  });
}

// refresh repository and call package info updates
void Chum::refreshRepo(bool force) {
  if (m_busy && !force) return;
  if (!m_ssu.repoAvailable()) {
      //% "Cannot refresh repository as it is not available"
      emit error(qtTrId("chum-refresh-repository-impossible"));
      return;
  }

  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  //% "Refreshing Chum repository"
  SET_STATUS(qtTrId("chum-refresh-repository"));

  auto pktr = Daemon::repoSetData(
    m_ssu.repoName(),
    QStringLiteral("refresh-now"),
    QVariant::fromValue(force).toString()
  );
  connect(pktr, &Transaction::finished, this, [this]() {
    SET_STATUS(QStringLiteral());
    refreshPackages();
    emit this->repositoryRefreshed();
  });
  connect(pktr, &Transaction::errorCode,
          [this](PackageKit::Transaction::Error /*error*/, const QString &details){
      qWarning() << "Failed to refresh Chum repository" << details;
      //% "Failed to refresh Chum repository"
      emit error(qtTrId("chum-refresh-repository-failed"));
  });
}

void Chum::repositoriesListUpdated() {
  if (!m_ssu.manageRepo()) {
      // repos are managed outside of GUI, probably misconfiguration
      emit errorFatal(
            //% "Repositories misconfigured"
            qtTrId("chum-repo-management-disabled-title"),
            //% "Cannot manage Chum repositories through GUI. You probably have multiple Chum "
            //% "repositories defined in SSU or Chum repository disabled. For listing repositories "
            //% "and their removal, use 'ssu' command in terminal.\n\n"
            //% "Please remove all defined Chum repositories and restart GUI. "
            //% "GUI will add missing Chum repository if needed on restart."
            qtTrId("chum-repo-management-disabled-txt"));
      return;
  } else if (!m_ssu.repoAvailable()) {
      m_busy = true;
      emit busyChanged();
      //% "Adding Chum repository"
      SET_STATUS(qtTrId("chum-add-repo"));
      m_ssu.setRepo();
      return;
  }
  refreshRepo(true);
}

void Chum::setRepoTesting(bool testing) {
  if (!m_ssu.manageRepo()) {
      emit error(qtTrId("chum-repo-management-disabled"));
      return;
  }

  if (!m_ssu.repoAvailable() || m_ssu.repoTesting() != testing) {
      m_busy = true;
      emit busyChanged();
      //% "Setting up Chum repository"
      SET_STATUS(qtTrId("chum-setup-repo"));
      m_ssu.setRepo(testing);
  }
}

// operations on packages
void Chum::installPackage(const QString &id) {
  if (m_busy) return;
  m_busy = true;
  emit busyChanged();
  //% "Install package"
  SET_STATUS(qtTrId("chum-install-package"));
  startOperation(Daemon::installPackage(id), id);
}

void Chum::updatePackage(const QString &id) {
  if (m_busy) return;
  m_busy = true;
  emit busyChanged();
  //% "Update package"
  SET_STATUS(qtTrId("chum-update-package"));
  startOperation(Daemon::updatePackage(id), id);
}

void Chum::startOperation(Transaction *pktr, const QString &pkg_id) {
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
    SET_STATUS(QStringLiteral());
    if (status == PackageKit::Transaction::ExitSuccess)
      emit this->packageOperationFinished(
        role2operation(pktr->role()),
        Daemon::packageName(pkg_id),
        Daemon::packageVersion(pkg_id)
      );
  });

  connect(pktr, &Transaction::errorCode,
          [this, pktr, pkg_id](PackageKit::Transaction::Error /*error*/, const QString &details){
      qWarning() << "Failed" << role2operation(pktr->role()) << pkg_id << details;
      emit error(details);
  });
}
