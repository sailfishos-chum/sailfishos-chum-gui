#include "chum.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

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

const QString Chum::repoName{QStringLiteral("sailfishos-chum")};

bool Chum::isChumPackage(const QString &id) {
  return Daemon::packageData(id) == repoName;
}

Chum::Chum(QObject *parent)
  : QObject{parent}
{
  connect(Daemon::global(), &Daemon::updatesChanged, this, &Chum::getUpdates);
}

void Chum::getUpdates() {
  static decltype(m_updates_count) new_count;
  new_count = 0;
  auto pktr = Daemon::getUpdates();
  connect(pktr, &Transaction::package, this, [](
    [[maybe_unused]] int info,
    const QString &packageID,
    [[maybe_unused]] const QString &summary
  ) {
    if (isChumPackage(packageID)) {
      ++new_count;
    }
  });
  connect(pktr, &Transaction::finished, this, [this]() {
    if (m_updates_count != new_count) {
      m_updates_count = new_count;
      emit this->updatesCountChanged();
    }
  });
}

void Chum::refreshRepo(bool force) {
  if (m_refreshing_repo) {
    return;
  }

  m_refreshing_repo = true;
  emit refreshingRepoChanged();

  auto pktr = Daemon::repoSetData(
    repoName,
    QStringLiteral("refresh-now"),
    QVariant::fromValue(force).toString()
  );
  connect(pktr, &Transaction::finished, this, [this]() {
    m_refreshing_repo = false;
    emit this->refreshingRepoChanged();
  });
}

void Chum::installPackage(const QString &id) {
  startOperation(Daemon::installPackage(id), id);
}

void Chum::updatePackage(const QString &id) {
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
    emit this->packageOperationFinished(
      role2operation(pktr->role()),
      Daemon::packageName(pkg_id),
      Daemon::packageVersion(pkg_id)
    );
  });
}
