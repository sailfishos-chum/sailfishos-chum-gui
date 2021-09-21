#include "chumpackage.h"

#include <PackageKit/Daemon>
#include <PackageKit/Details>

using namespace PackageKit;

ChumPackage::ChumPackage(QObject *parent)
  : QObject{parent}
{
}

void ChumPackage::setPkid(const QString &pkid) {
  if (m_pkid == pkid) {
    return;
  }

  m_pkid = pkid;
  emit pkidChanged();
  if (m_pkid.isEmpty()) {
    return;
  }

  auto details_tr = Daemon::getDetails(m_pkid);
  connect(details_tr, &Transaction::finished, details_tr, &Transaction::deleteLater);
  connect(details_tr, &Transaction::details, this, [this](const auto &v) {
    m_description = v.description();
    m_summary     = v.summary();
    m_url         = v.url();
    m_license     = v.license();
    m_size        = v.size();
    emit updated();
  });

  auto resolve_tr = Daemon::resolve(Daemon::packageName(m_pkid), Transaction::FilterInstalled);
  connect(resolve_tr, &Transaction::finished, resolve_tr, &Transaction::deleteLater);
  connect(resolve_tr, &Transaction::package, this, [this](
    [[maybe_unused]] auto info,
    const auto &packageID,
    [[maybe_unused]] const auto &summary
  ) {
    m_installed_version = Daemon::packageVersion(packageID);
    emit installedVersionChanged();
  });
}

void ChumPackage::installPackage() {
  if (m_pkid.isEmpty()) {
    return;
  }

  auto pktr = Daemon::installPackage(m_pkid);
  connect(pktr, &Transaction::finished, pktr, &Transaction::deleteLater);
}

void ChumPackage::updatePackage() {
  if (m_pkid.isEmpty()) {
    return;
  }

  auto pktr = Daemon::updatePackage(m_pkid);
  connect(pktr, &Transaction::finished, pktr, &Transaction::deleteLater);
}
