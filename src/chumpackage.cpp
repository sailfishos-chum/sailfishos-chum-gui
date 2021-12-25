#include "chumpackage.h"

#include "projectgithub.h"
#include "projectgitlab.h"

#include <PackageKit/Daemon>
#include <PackageKit/Details>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <yaml-cpp/yaml.h>

using namespace PackageKit;

#define SET_IF_EMPTY(var, role, value) { \
  if (!var.isEmpty() || var==value) return; \
  var = value; \
  emit updated(m_id, role); \
}


ChumPackage::ChumPackage(QObject *parent)
  : QObject{parent}
{
  m_issue_info = new LoadableObject(this);
  m_issues = new LoadableObject(this);
  m_release_info = new LoadableObject(this);
  m_releases = new LoadableObject(this);
}


ChumPackage::ChumPackage(const QString &id, QObject *parent)
  : ChumPackage(parent)
{
  m_id = id;
}

QString ChumPackage::developer() const {
  if (!m_developer_login.isEmpty() && !m_developer_name.isEmpty())
    return QStringLiteral("%1 (%2)").arg(m_developer_name, m_developer_login);
  if (!m_developer_login.isEmpty()) return m_developer_login;
  if (!m_developer_name.isEmpty()) return m_developer_name;
  return QString();
}

bool ChumPackage::installed() const {
  return !m_installed_version.isEmpty();
}

LoadableObject* ChumPackage::issue(const QString &id) {
  if (m_project != nullptr)
    m_project->issue(id, m_issue_info);
  else
    m_issue_info->setEmpty();
  return m_issue_info;
}

LoadableObject* ChumPackage::issues() {
  if (m_issues->ready()) return m_issues;
  if (m_project != nullptr)
    m_project->issues(m_issues);
  else
    m_issues->setEmpty();
  return m_issues;
}

LoadableObject* ChumPackage::release(const QString &id) {
  if (m_project != nullptr)
    m_project->release(id, m_release_info);
  else
    m_release_info->setEmpty();
  return m_release_info;
}

LoadableObject* ChumPackage::releases() {
  if (m_releases->ready()) return m_releases;
  if (m_project != nullptr)
    m_project->releases(m_releases);
  else
    m_releases->setEmpty();
  return m_releases;
}

void ChumPackage::setPkidLatest(const QString &pkid) {
  if (m_pkid_latest == pkid) return;

  m_pkid_latest = pkid;

  if (m_pkid_latest.isEmpty()) {
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

  m_available_version = Daemon::packageVersion(v.packageId());
  m_description = v.description();
  m_summary     = v.summary();
  m_url         = v.url();
  m_license     = v.license();
  m_size        = v.size();

  // derive name
  m_name = Daemon::packageName(m_pkid_latest);
  for (const QLatin1String &prefix: {QLatin1String("harbour-"), QLatin1String("openrepos-")})
    if (m_name.startsWith(prefix))
      m_name = m_name.mid(prefix.size());
  bool capitalize = true;
  const QChar sep{'-'};
  const QChar space{' '};
  for (auto begin = m_name.begin(), end = m_name.end(), it = begin; it != end; ++it) {
    if (capitalize) {
      *it = it->toUpper();
      capitalize = false;
    } else if (it != begin && *it == sep) {
      *it = space;
      capitalize = true;
    }
  }
  m_name = m_name.trimmed();

  // parse description
  QStringList descLines = m_description.split(QRegularExpression("(?m)^\\s*$"), QString::SkipEmptyParts);

  QByteArray metainjson;
  YAML::Node metayaml;

  try {
      metayaml = YAML::Load(descLines.last().toStdString());
  } catch(const YAML::ParserException &e) {
      qWarning() << "Invalid Chum section for" << m_pkid_latest;
  }

  if (!metayaml.IsNull() && metayaml.size() > 0) {
      YAML::Emitter emitter;
      emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << metayaml;
      std::string out(emitter.c_str() + 1);  // Strip leading [ character
      metainjson = QByteArray::fromStdString(out);
      metainjson = metainjson.replace("~", "\"\"");

      //remove yaml from list
      descLines.pop_back();
  }

  // drop newlines from description paragraphs
  for (int i=0; i < descLines.length(); ++i)
    descLines[i] = descLines[i].replace('\n', ' ').simplified();

  // Reconstruct the description
  m_description = descLines.join("\n\n");

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

  for (const QString &u: {m_repo_url, m_url}) {
      if (ProjectGitHub::isProject(u))
        m_project = new ProjectGitHub(u, this);
      else if (ProjectGitLab::isProject(u))
        m_project = new ProjectGitLab(u, this);
      if (m_project) break;
  }

  emit updated(m_id, PackageRefreshRole);
}

void ChumPackage::setPkidInstalled(const QString &pkid) {
  m_installed_update = false;
  if (m_pkid_installed == pkid) return;
  m_pkid_installed = pkid;
  setInstalledVersion(Daemon::packageVersion(pkid));
}

void ChumPackage::setInstalledVersion(const QString &v)
{
  if (v == m_installed_version) return;
  m_installed_version = v;
  emit updated(m_id, PackageInstalledVersionRole);
  emit updated(m_id, PackageInstalledRole);
}

void ChumPackage::setDeveloperLogin(const QString &login) {
  SET_IF_EMPTY(m_developer_login, PackageDeveloperLoginRole, login);
}

void ChumPackage::setDeveloperName(const QString &name) {
  SET_IF_EMPTY(m_developer_name, PackageDeveloperLoginRole, name);
}

void ChumPackage::setUrl(const QString &url) {
  SET_IF_EMPTY(m_url, PackageOtherRole, url);
}

void ChumPackage::setUrlForum(const QString &url) {
  SET_IF_EMPTY(m_url_forum, PackageOtherRole, url);
}

void ChumPackage::setUrlIssues(const QString &url) {
  SET_IF_EMPTY(m_url_issues, PackageOtherRole, url);
}

void ChumPackage::setForksCount(int count) {
  m_forks_count = count;
  emit updated(m_id, PackageOtherRole);
}

void ChumPackage::setIssuesCount(int count) {
  m_issues_count = count;
  emit updated(m_id, PackageOtherRole);
}

void ChumPackage::setReleasesCount(int count) {
  m_releases_count = count;
  emit updated(m_id, PackageOtherRole);
}

void ChumPackage::setStarsCount(int count) {
  m_stars_count = count;
  emit updated(m_id, PackageStarsCountRole);
}
