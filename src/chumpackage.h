#pragma once

#include <QObject>
#include <PackageKit/Details>

class ChumPackage : public QObject {
  Q_OBJECT

  // TODO: properties should be all read only, setters are used from C++
  Q_PROPERTY(QString id READ id NOTIFY pkidChanged)
  Q_PROPERTY(bool    updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)

  Q_PROPERTY(QString    description READ description  NOTIFY updated)
  Q_PROPERTY(QString    installedVersion READ installedVersion NOTIFY updated)
  Q_PROPERTY(QString    license     READ license      NOTIFY updated)
  Q_PROPERTY(QString    metadata    READ metadata     NOTIFY updated)
  Q_PROPERTY(QString    name        READ name         NOTIFY updated)
  Q_PROPERTY(qulonglong size        READ size         NOTIFY updated)
  Q_PROPERTY(QString    summary     READ summary      NOTIFY updated)
  Q_PROPERTY(QString    url         READ url          NOTIFY updated)

public:
  enum Role {
    PackageIdRole = Qt::UserRole + 1,
    PackageNameRole,
    PackageInstalledVersionRole,
    PackageSummaryRole,

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

  QString description() const { return m_description; }
  QString installedVersion() const { return m_installed_version; }
  QString license() const { return m_license; }
  QString metadata() const { return m_metadata; }
  QString name() const { return m_name; }
  qulonglong size() const { return m_size; }
  QString summary() const { return m_summary; }
  QString url() const { return m_url; }

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

  QString     m_description;
  QString     m_license;
  QString     m_metadata;
  QString     m_name;
  qulonglong  m_size{0};
  QString     m_summary;
  QString     m_url;
};
