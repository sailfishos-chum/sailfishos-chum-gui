#include "chumpackage.h"

#include <PackageKit/Daemon>
#include <PackageKit/Details>
#include <QDebug>
#include <QRegularExpression>
#include <yaml-cpp/yaml.h>

using namespace PackageKit;

ChumPackage::ChumPackage(QObject *parent)
  : QObject{parent}
{
}


ChumPackage::ChumPackage(QString id, QObject *parent)
  : QObject{parent}, m_id(id)
{
}

void ChumPackage::setPkid(const QString &pkid) {
  if (m_pkid == pkid) return;

  m_pkid = pkid;
  emit pkidChanged();

  if (m_pkid.isEmpty()) {
    m_details_update = false;
    m_installed_update = false;
    return;
  }

  m_details_update = true;
  m_installed_update = true;
}

void ChumPackage::setUpdateAvailable(bool up) {
  if (up == m_update_available) return;
  m_update_available = up;
  emit updated(m_id, PackageUpdateAvailableRole);
}

void ChumPackage::setDetails(const PackageKit::Details &v) {
  m_details_update = false;

  m_description = v.description();
  m_summary     = v.summary();
  m_url         = v.url();
  m_license     = v.license();
  m_size        = v.size();

  // derive name
  m_name = Daemon::packageName(m_pkid);
  for (QString prefix: {"harbour-", "openrepos-"})
    if (m_name.startsWith(prefix))
      m_name = m_name.mid(prefix.length());
  QStringList nparts = m_name.split('-');
  m_name = QString{};
  for (const QString& b: nparts)
    m_name += b.left(1).toUpper() + b.mid(1).toLower() + " ";
  m_name = m_name.trimmed();

  // parse description
  QStringList descLines = m_description.split(QRegularExpression("(?m)^\\s*$"), QString::SkipEmptyParts);

  YAML::Node meta;

  try {
      meta = YAML::Load(descLines.last().toStdString());
  } catch(const YAML::ParserException &e) {
      qWarning() << "Invalid Chum section for" << m_pkid;
  }

  qDebug() << "Node size:" << meta.size();
  if (!meta.IsNull() && meta.size() > 0) {
      YAML::Emitter emitter;
      emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << meta;
      std::string out(emitter.c_str() + 1);  // Strip leading [ character
      m_metadata = QString::fromStdString(out);
      m_metadata = m_metadata.replace("~", "\"\"");
      qDebug() << "metadata:\n" << m_metadata << '\n';

      //remove yaml from list
      descLines.pop_back();

      //Reconstruct the description
      m_description = descLines.join("\n\n");
  }

  emit updated(m_id, PackageRefreshRole);
}

void ChumPackage::setInstalledVersion(const QString &v)
{
  m_installed_update = false;
  if (v == m_installed_version) return;
  m_installed_version = v;
  emit updated(m_id, PackageInstalledVersionRole);
}
