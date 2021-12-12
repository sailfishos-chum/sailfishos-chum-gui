#include "chum.h"

#include <PackageKit/Daemon>

#include <QDebug>

using namespace PackageKit;

Chum* Chum::s_instance{nullptr};
const QString Chum::s_repo_name{QStringLiteral(REPO_ALIAS)};

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
  : QObject{parent} {
  connect(Daemon::global(), &Daemon::updatesChanged, [this]() { this->getUpdates(); });
  refreshPackages();
}

Chum* Chum::instance() {
  if (!s_instance) s_instance = new Chum();
  return s_instance;
}

bool Chum::isChumPackage(const QString &id) {
  return Daemon::packageData(id) == s_repo_name;
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

  auto pktr = Daemon::getPackages(Transaction::FilterNotDevel);
  connect(pktr, &Transaction::package,  this, [this](
          [[maybe_unused]] int info,
          const QString &packageID,
          [[maybe_unused]] const QString &summary
          ) {
    if (isChumPackage(packageID))
      m_packages_last_refresh.insert(packageID);
  });
  connect(pktr, &Transaction::finished, this, &Chum::refreshPackagesFinished);
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

  refreshDetails();
}

void Chum::refreshDetails() {
  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

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
     this->refreshInstalledVersion();
  });
}

void Chum::refreshInstalledVersion() {
  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  QStringList packages;
  for (ChumPackage *p: m_packages.values())
    if (p->installedVersionNeedsUpdate()) {
      packages.append(Daemon::packageName(p->pkid()));
      // reset requirement flag that can be set for non-installed package
      p->setInstalledVersion(p->installedVersion());
    }

  auto tr = Daemon::resolve(packages, Transaction::FilterInstalled);
  connect(tr, &Transaction::package, this, [this](
          [[maybe_unused]] auto info,
          const auto &packageID,
          [[maybe_unused]] const auto &summary) {
    ChumPackage *p = m_packages.value(packageId(packageID), nullptr);
    if (p)
      p->setInstalledVersion(Daemon::packageVersion(packageID));
    else
      qWarning() << "Got installed version for missing package:" << packageID;
  });

  connect(tr, &Transaction::finished, this, [this]() {
     this->getUpdates(true);;
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
    if (isChumPackage(packageID)) {
      ChumPackage *p = m_packages.value(packageId(packageID), nullptr);
      if (p)
        p->setUpdateAvailable(true);
      else
        qWarning() << "Package" << packageID << "missing among the current packages";
      ++new_count;
    }
  });
  connect(pktr, &Transaction::finished, this, [this]() {
    if (m_updates_count != new_count) {
      m_updates_count = new_count;
      emit this->updatesCountChanged();
    }
    m_busy = false;
    emit busyChanged();
    emit packagesChanged();
  });
}

// refresh repository and call package info updates
void Chum::refreshRepo(bool force) {
  if (m_busy && !force) return;

  if (!m_busy) {
    m_busy = true;
    emit busyChanged();
  }

  auto pktr = Daemon::repoSetData(
    s_repo_name,
    QStringLiteral("refresh-now"),
    QVariant::fromValue(force).toString()
  );
  connect(pktr, &Transaction::finished, this, [this]() {
    refreshPackages();
    emit this->repositoryRefreshed();
  });
}

// operations on packages
void Chum::installPackage(const QString &id) {
  if (m_busy) return;
  m_busy = true;
  emit busyChanged();
  startOperation(Daemon::installPackage(id), id);
}

void Chum::updatePackage(const QString &id) {
  if (m_busy) return;
  m_busy = true;
  emit busyChanged();
  startOperation(Daemon::updatePackage(id), id);
}

void Chum::startOperation(Transaction *pktr, const QString &pkg_id) {
  connect(pktr, &Transaction::roleChanged, this, [this, pktr, pkg_id]() {
    emit this->packageOperationStarted(
      role2operation(pktr->role()),
      Daemon::packageName(pkg_id)
    );
  });

  connect(pktr, &Transaction::finished, this, [this, pktr, pkg_id]() {
    m_busy = false;
    emit busyChanged();
    emit this->packageOperationFinished(
      role2operation(pktr->role()),
      Daemon::packageName(pkg_id),
      Daemon::packageVersion(pkg_id)
    );
  });
}
