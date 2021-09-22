#include "chumupdatesmodel.h"

#include <PackageKit/Daemon>

using namespace PackageKit;

ChumUpdatesModel::ChumUpdatesModel(QObject *parent)
  : ChumAbstractPackageModel{parent}
{
}

void ChumUpdatesModel::reset() {
  beginResetModel();
  auto pktr = Daemon::getUpdates();
  connect(pktr, &Transaction::package,  this, &ChumUpdatesModel::insertPackage);
  connect(pktr, &Transaction::finished, this, &ChumUpdatesModel::endResetModel);
}
