#include "chumpackage.h"

#include <PackageKit/Daemon>
#include <PackageKit/Details>
#include <QDebug>

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
  connect(details_tr, &Transaction::details, this, [this](const auto &v) {
    m_description = v.description();
    m_summary     = v.summary();
    m_url         = v.url();
    m_license     = v.license();
    m_size        = v.size();

    int idx = m_description.indexOf("BEGINCHUMMETADATA");
    if (idx >= 0) {
        YAML::Node meta = YAML::Load(m_description.mid(idx + 17).toStdString());

        YAML::Emitter emitter;
        emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << meta;
        std::string out(emitter.c_str() + 1);  // Strip leading [ character
        m_metadata = QString::fromStdString(out);
        m_metadata = m_metadata.replace("~", "\"\"");
        qDebug() << "metadata:\n" << m_metadata << '\n';

        m_description = m_description.left(idx - 1);

    }
    emit this->updated();
  });

  auto resolve_tr = Daemon::resolve(Daemon::packageName(m_pkid), Transaction::FilterInstalled);
  connect(resolve_tr, &Transaction::package, this, [this](
    [[maybe_unused]] auto info,
    const auto &packageID,
    [[maybe_unused]] const auto &summary
  ) {
    m_installed_version = Daemon::packageVersion(packageID);
    emit this->installedVersionChanged();
  });
}
