#include "chumpackagemodel.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

const auto CHUM = QStringLiteral("sailfishos-chum");

ChumPackageModel::ChumPackageModel(QObject *parent)
  : QAbstractListModel{parent}
{
  reset();
}

int ChumPackageModel::rowCount(const QModelIndex &parent) const {
  return !parent.isValid() ? m_data.size() : 0;
}

QVariant ChumPackageModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant{};
  }

  const auto &p = m_data[index.row()];
  switch (role) {
  case PackageIdRole:
    return p.id;
  case PackageNameRole:
    return p.name;
  case PackageVersionRole:
    return p.version;
  default:
    return QVariant{};
  }
}

QHash<int, QByteArray> ChumPackageModel::roleNames() const {
  return {
    {PackageIdRole,       QByteArrayLiteral("packageId")},
    {PackageNameRole,     QByteArrayLiteral("packageName")},
    {PackageVersionRole,  QByteArrayLiteral("packageVersion")},
  };
}

void ChumPackageModel::reset() {
  beginResetModel();
  auto pktr = Daemon::getPackages(Transaction::FilterNotDevel);
  connect(pktr, &Transaction::package, this, [this]([[maybe_unused]] auto info, const auto &packageID, [[maybe_unused]] const auto &summary) {
    const auto repo = Daemon::packageData(packageID);
    if (repo == CHUM) {
      m_data.push_back(Package {
        .id      = packageID,
        .name    = Daemon::packageName(packageID),
        .version = Daemon::packageVersion(packageID)
      });
    }
  });
  connect(pktr, &Transaction::finished, this, [this, pktr]() {
    pktr->deleteLater();
    endResetModel();
  });
}
