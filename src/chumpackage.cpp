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

  auto pktr = Daemon::getDetails(m_pkid);
  connect(pktr, &Transaction::finished, pktr, &Transaction::deleteLater);
  connect(pktr, &Transaction::details, this, [this](const auto &v) {
    m_description = v.description();
    m_summary     = v.summary();
    m_url         = v.url();
    m_license     = v.license();
    m_size        = v.size();
    emit updated();
  });
}

void ChumPackage::install() {
  if (m_pkid.isEmpty()) {
    return;
  }

  auto pktr = Daemon::installPackage(m_pkid);
  connect(pktr, &Transaction::finished, pktr, &Transaction::deleteLater);
}
