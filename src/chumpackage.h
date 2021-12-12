#pragma once

#include <QObject>
#include <PackageKit/Details>

class ChumPackage : public QObject {
  Q_OBJECT

  // TODO: properties should be all read only, setters are used from C++
  Q_PROPERTY(QString pkid READ pkid WRITE setPkid NOTIFY pkidChanged)
  Q_PROPERTY(QString installedVersion READ installedVersion NOTIFY installedVersionChanged)
  Q_PROPERTY(bool    updateAvailable READ updateAvailable NOTIFY updateAvailableChanged)
  Q_PROPERTY(QString    description MEMBER m_description  NOTIFY updated)
  Q_PROPERTY(QString    summary     MEMBER m_summary      NOTIFY updated)
  Q_PROPERTY(QString    url         MEMBER m_url          NOTIFY updated)
  Q_PROPERTY(QString    license     MEMBER m_license      NOTIFY updated)
  Q_PROPERTY(QString    metadata    MEMBER m_metadata     NOTIFY updated)
  Q_PROPERTY(qulonglong size        MEMBER m_size         NOTIFY updated)

public:
  explicit ChumPackage(QObject *parent = nullptr);

  QString pkid() const { return m_pkid; }
  bool updateAvailable() const { return m_update_available; }
  QString installedVersion() const { return m_installed_version; }
  bool detailsNeedsUpdate() const { return m_details_update; }
  bool installedVersionNeedsUpdate() const { return m_installed_update; }

  void setPkid(const QString &pkid);
  void setUpdateAvailable(bool up);
  void setDetails(const PackageKit::Details &v);
  void setInstalledVersion(const QString &v);

signals:
  void updated();
  void pkidChanged();
  void installedVersionChanged();
  void updateAvailableChanged();

private:
  QString     m_pkid;
  QString     m_installed_version;
  bool        m_update_available{false};
  bool        m_details_update{false};
  bool        m_installed_update{false};
  QString     m_description;
  QString     m_summary;
  QString     m_url;
  QString     m_license;
  qulonglong  m_size{0};
  QString     m_metadata;
};
