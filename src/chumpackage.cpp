#include "chumpackage.h"

#include <PackageKit/Daemon>
#include <PackageKit/Details>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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

  QByteArray metainjson;
  YAML::Node metayaml;

  try {
      metayaml = YAML::Load(descLines.last().toStdString());
  } catch(const YAML::ParserException &e) {
      qWarning() << "Invalid Chum section for" << m_pkid;
  }

  if (!metayaml.IsNull() && metayaml.size() > 0) {
      YAML::Emitter emitter;
      emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << metayaml;
      std::string out(emitter.c_str() + 1);  // Strip leading [ character
      metainjson = QByteArray::fromStdString(out);
      metainjson = metainjson.replace("~", "\"\"");

      qDebug() << m_pkid << "Meta:" << metainjson;

      //remove yaml from list
      descLines.pop_back();

      //Reconstruct the description
      m_description = descLines.join("\n\n");
  }

  // Parse metadata
  QJsonObject json{QJsonDocument::fromJson(metainjson).object()};
  m_name = json.value("PackageName").toString(m_name);
  m_type = json.value("Type").toString(m_id.startsWith(QStringLiteral("harbour-")) ?
                                                         QStringLiteral("desktop-application") :
                                                         QStringLiteral("generic"));
  m_developer_name = json.value("DeveloperName").toString();
  m_categories = json.value("Categories").toVariant().toStringList();
  if (m_categories.isEmpty()) m_categories.push_back("Other");

  m_repo_type = json.value("Custom").toObject().value("RepoType").toString();
  m_repo_url = json.value("Custom").toObject().value("Repo").toString();

  m_icon = json.value("Icon").toString();
  m_screenshots = json.value("Screenshots").toVariant().toStringList();

  m_url = json.value("Url").toObject().value("Homepage").toString(m_url);
  m_url_forum = json.value("Url").toObject().value("Help").toString();
  m_url_issues = json.value("Url").toObject().value("Bugtracker").toString();
  m_donation = json.value("Url").toObject().value("Donation").toString();

  emit updated(m_id, PackageRefreshRole);
}

void ChumPackage::setInstalledVersion(const QString &v)
{
  m_installed_update = false;
  if (v == m_installed_version) return;
  m_installed_version = v;
  emit updated(m_id, PackageInstalledVersionRole);
}
