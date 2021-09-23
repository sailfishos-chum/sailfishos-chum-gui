#pragma once

#include <QObject>

class ChumPackage : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString pkid READ pkid WRITE setPkid NOTIFY pkidChanged)
  Q_PROPERTY(QString installedVersion MEMBER m_installed_version NOTIFY installedVersionChanged)
  Q_PROPERTY(QString    description MEMBER m_description  NOTIFY updated)
  Q_PROPERTY(QString    summary     MEMBER m_summary      NOTIFY updated)
  Q_PROPERTY(QString    url         MEMBER m_url          NOTIFY updated)
  Q_PROPERTY(QString    license     MEMBER m_license      NOTIFY updated)
  Q_PROPERTY(qulonglong size        MEMBER m_size         NOTIFY updated)

public:
  explicit ChumPackage(QObject *parent = nullptr);

  QString pkid() const {
    return m_pkid;
  }

  void setPkid(const QString &pkid);

signals:
  void updated();
  void pkidChanged();
  void installedVersionChanged();

private:
  QString     m_pkid;
  QString     m_installed_version;
  QString     m_description;
  QString     m_summary;
  QString     m_url;
  QString     m_license;
  qulonglong  m_size{0};
};
