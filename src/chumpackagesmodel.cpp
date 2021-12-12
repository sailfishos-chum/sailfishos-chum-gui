#include "chumpackagesmodel.h"
#include "chum.h"

#include <algorithm>

ChumPackagesModel::ChumPackagesModel(QObject *parent)
  : QAbstractListModel{parent}
{
  connect(Chum::instance(), &Chum::packagesChanged, this, &ChumPackagesModel::reset);
}

int ChumPackagesModel::rowCount(const QModelIndex &parent) const {
  return !parent.isValid() ? m_packages.size() : 0;
}

QVariant ChumPackagesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_packages.size()) {
    return QVariant{};
  }

  const ChumPackage *p = Chum::instance()->package(m_packages[index.row()]);
  switch (role) {
  case ChumPackage::PackageIdRole:
    return p->id();
  case ChumPackage::PackageNameRole:
    return p->name();
  case ChumPackage::PackageInstalledVersionRole:
    return p->installedVersion();
  default:
    return QVariant{};
  }
}

QHash<int, QByteArray> ChumPackagesModel::roleNames() const {
  return {
    {ChumPackage::PackageIdRole,       QByteArrayLiteral("packageId")},
    {ChumPackage::PackageNameRole,     QByteArrayLiteral("packageName")},
    {ChumPackage::PackageInstalledVersionRole,  QByteArrayLiteral("packageInstalledVersion")},
  };
}

void ChumPackagesModel::reset() {
  beginResetModel();

  m_packages.clear();
  QList<ChumPackage*> packages;

  // filter packages
  for (ChumPackage* p: Chum::instance()->packages()) {
    disconnect(p, nullptr, this, nullptr);
    // TODO apply filters, such as category, updatable, search query

    // add to filtered packages and follow package updates
    packages.push_back(p);
    connect(p, &ChumPackage::updated, this, &ChumPackagesModel::updatePackage);
  }

  // sort packages
  std::sort(packages.begin(), packages.end(), [](const ChumPackage *a, const ChumPackage *b){
    return a->name() < b->name();
  });

  // record the result
  for (const ChumPackage* p: packages)
    m_packages.push_back(p->id());

  endResetModel();
}

void ChumPackagesModel::updatePackage(QString packageId, ChumPackage::Role role) {
  // check if update is of interest
  QList<ChumPackage::Role> roles{
    ChumPackage::PackageRefreshRole,
    ChumPackage::PackageIdRole,
    ChumPackage::PackageNameRole,
    ChumPackage::PackageInstalledVersionRole
  };

  QList<ChumPackage::Role> search_roles{
    ChumPackage::PackageRefreshRole,
    ChumPackage::PackageNameRole
  };

  QList<ChumPackage::Role> sort_roles{
    ChumPackage::PackageRefreshRole,
    ChumPackage::PackageNameRole
  };

  // skip the roles we don't follow and when chum reposirory is refreshed
  if (!roles.contains(role) ||
      (role == ChumPackage::PackageRefreshRole && Chum::instance()->busy()))
    return;

  // call reset if some package was reporting refreshrole
  if (role==ChumPackage::PackageRefreshRole) {
      reset();
      return;
  }

  // check if it can trigger any of the filters
  bool filter_or_order_may_change = false;
  if (!m_search.isEmpty() && search_roles.contains(role))
    filter_or_order_may_change = true;
  // TODO: other filters

  // check if sorting maybe altered
  if (sort_roles.contains(role))
    filter_or_order_may_change = true;

  if (filter_or_order_may_change) {
      reset();
      return;
  }

  // minor change and invalidate corresponding cell
  int i = m_packages.indexOf(packageId);
  if (i < 0) return; // no such package
  dataChanged(index(i), index(i) ); // just refresh whole row to simplify processing here
}

void ChumPackagesModel::setSearch(QString search) {
  if (search == m_search) return;
  emit searchChanged();
  reset();
}
