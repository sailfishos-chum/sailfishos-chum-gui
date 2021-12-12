#include "chumabstractpackagemodel.h"
#include "chum.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

ChumAbstractPackageModel::ChumAbstractPackageModel(QObject *parent)
  : QAbstractListModel{parent}
{
}

int ChumAbstractPackageModel::rowCount(const QModelIndex &parent) const {
  return !parent.isValid() ? m_data.size() : 0;
}

QVariant ChumAbstractPackageModel::data(const QModelIndex &index, int role) const {
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
  case PackageSummaryRole:
    return p.summary;
  default:
    return QVariant{};
  }
}

QHash<int, QByteArray> ChumAbstractPackageModel::roleNames() const {
  return {
    {PackageIdRole,       QByteArrayLiteral("packageId")},
    {PackageNameRole,     QByteArrayLiteral("packageName")},
    {PackageVersionRole,  QByteArrayLiteral("packageVersion")},
    {PackageSummaryRole,  QByteArrayLiteral("packageSummary")},
  };
}

void ChumAbstractPackageModel::insertPackage(
  [[maybe_unused]] int info,
  const QString &packageID,
  const QString &summary
) {
  if (Chum::isChumPackage(packageID)) {
    m_data.push_back(Package {
      .id      = packageID,
      .name    = Daemon::packageName(packageID),
      .version = Daemon::packageVersion(packageID),
      .summary = summary,
    });
  }
}
