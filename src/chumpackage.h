#pragma once

#include <QObject>
#include <PackageKit/Details>

class ChumPackage : public QObject {
  Q_OBJECT

  // TODO: properties should be all read only, setters are used from C++
  Q_PROPERTY(QString id READ id NOTIFY pkidChanged)
  Q_PROPERTY(bool    updateAvailable READ updateAvailable NOTIFY updated)

  Q_PROPERTY(QStringList categories READ categories   NOTIFY updated)
  Q_PROPERTY(QString    description READ description  NOTIFY updated)
  Q_PROPERTY(QString    developerName READ developerName  NOTIFY updated)
  Q_PROPERTY(QString    donation    READ donation     NOTIFY updated)
  Q_PROPERTY(QString    icon        READ icon         NOTIFY updated)
  Q_PROPERTY(QString    installedVersion READ installedVersion NOTIFY updated)
  Q_PROPERTY(QString    license     READ license      NOTIFY updated)
  Q_PROPERTY(QString    name        READ name         NOTIFY updated)
  Q_PROPERTY(QString    repo        READ repo         NOTIFY updated)
  Q_PROPERTY(QStringList screenshots READ screenshots NOTIFY updated)
  Q_PROPERTY(qulonglong size        READ size         NOTIFY updated)
  Q_PROPERTY(QString    summary     READ summary      NOTIFY updated)
  Q_PROPERTY(QString    type        READ type         NOTIFY updated)
  Q_PROPERTY(QString    url         READ url          NOTIFY updated)
  Q_PROPERTY(QString    urlForum    READ urlForum     NOTIFY updated)
  Q_PROPERTY(QString    urlIssues   READ urlIssues    NOTIFY updated)

public:
  enum Role {
    PackageIdRole = Qt::UserRole + 1,
    PackageDeveloperName,
    PackageIconRole,
    PackageInstalledVersionRole,
    PackageNameRole,
    PackageSummaryRole,
    PackageTypeRole,
    PackageUpdateAvailableRole,

    PackageOtherRole,
    PackageRefreshRole // used for updates of many parameters
  };

  ChumPackage(QObject *parent = nullptr);
  ChumPackage(QString id, QObject *parent = nullptr);

  QString pkid() const { return m_pkid; }
  QString id() const { return m_id; }
  bool updateAvailable() const { return m_update_available; }
  bool detailsNeedsUpdate() const { return m_details_update; }
  bool installedVersionNeedsUpdate() const { return m_installed_update; }

  QStringList categories() const { return m_categories; }
  QString description() const { return m_description; }
  QString developerName() const { return m_developer_name; }
  QString donation() const { return m_donation; }
  QString icon() const { return m_icon; }
  QString installedVersion() const { return m_installed_version; }
  QString license() const { return m_license; }
  QString name() const { return m_name; }
  QString repo() const { return m_repo_url; }
  QStringList screenshots() const { return m_screenshots; }
  qulonglong size() const { return m_size; }
  QString summary() const { return m_summary; }
  QString type() const { return m_type; }
  QString url() const { return m_url; }
  QString urlForum() const { return m_url_forum; }
  QString urlIssues() const { return m_url_issues; }

  void setPkid(const QString &pkid);
  void setUpdateAvailable(bool up);
  void setDetails(const PackageKit::Details &v);
  void setInstalledVersion(const QString &v);

signals:
  void updated(QString packageId, Role role);
  void pkidChanged();
  void updateAvailableChanged();

private:
  QString     m_id; // ID of the package as used in Chum
  QString     m_pkid; // Package ID as set by PackageKit
  QString     m_installed_version;
  bool        m_update_available{false};
  bool        m_details_update{false};
  bool        m_installed_update{false};

  QStringList m_categories;
  QString     m_description;
  QString     m_developer_name;
  QString     m_donation;
  QString     m_icon;
  QString     m_license;
  QString     m_name;
  QString     m_repo_type;
  QString     m_repo_url;
  QStringList m_screenshots;
  qulonglong  m_size{0};
  QString     m_summary;
  QString     m_type;
  QString     m_url;
  QString     m_url_forum;
  QString     m_url_issues;
};
