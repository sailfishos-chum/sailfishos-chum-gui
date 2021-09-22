#include "chum.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

const QString Chum::repoName{QStringLiteral("sailfishos-chum")};

Chum::Chum(QObject *parent)
  : QObject{parent}
{
  auto pk = Daemon::global();
  connect(pk, &Daemon::updatesChanged, this, &Chum::getUpdates);
}

bool Chum::isChumPackage(const QString &id) {
  return Daemon::packageData(id) == repoName;
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
      emit updatesCountChanged();
    }
  });
}
