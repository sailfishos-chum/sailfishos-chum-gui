#include "chumavailablepackagesmodel.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

ChumAvailablePackagesModel::ChumAvailablePackagesModel(QObject *parent)
  : ChumAbstractPackageModel{parent}
{
}

void ChumAvailablePackagesModel::reset() {
  beginResetModel();
  auto pktr = Daemon::getPackages(Transaction::FilterNotDevel);
  connect(pktr, &Transaction::package,  this, &ChumAvailablePackagesModel::insertPackage);
  connect(pktr, &Transaction::finished, this, &ChumAvailablePackagesModel::endResetModel);
}
