#include "chum.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

const QString Chum::repoName{QStringLiteral("sailfishos-chum")};

bool Chum::isChumPackage(const QString &id) {
  return Daemon::packageData(id) == repoName;
}

Chum::Chum(QObject *parent)
  : QObject{parent}
{
  auto pk = Daemon::global();
  connect(pk, &Daemon::updatesChanged, this, &Chum::getUpdates);
//  connect(pk, &Daemon::transactionListChanged, this, [this](
//    const auto &tids
//  ) {
//    for (const auto &tid : tids) {
//      if (!m_tids.contains(tid)) {
//        auto pktr = new Transaction{tid};
//        connect(pktr, &Transaction::finished, this, [this, pktr]() {
//          switch (pktr->role()) {
//          case Transaction::RoleInstallPackages:
//            pktr
//          }
//        });
//      }
//    }
//    m_tids = tids.toSet();
//  });
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
