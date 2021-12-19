#pragma once

#include <QHash>
#include <QObject>
#include <QSet>

#include "chumpackage.h"

namespace PackageKit {
class Transaction;
}

class Chum : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool    busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(QString status READ status NOTIFY statusChanged)
  Q_PROPERTY(quint32 updatesCount   READ updatesCount NOTIFY updatesCountChanged)

public:
  enum PackageOperation {
    PackageUnknownOperation,
    PackageInstallation,
    PackageUpdate,
  };
  Q_ENUM(PackageOperation)

  // repo operations
  bool busy() const { return m_busy; }
  QString status() const { return m_status; }
  quint32 updatesCount() const { return m_updates_count; }

  QList<ChumPackage*> packages() const { return m_packages.values(); }
  Q_INVOKABLE ChumPackage* package(const QString &id) const { return m_packages.value(id, nullptr); }

  // static public methods
  static Chum* instance();
  static bool isChumPackage(const QString &id);

public slots:
  void getUpdates(bool force = false);
  void refreshRepo(bool force = false);
  void installPackage(const QString &id);
  void updatePackage(const QString &id);

signals:
  void busyChanged();
  void error(QString errorTxt);
  void statusChanged();
  void updatesCountChanged();
  void packagesChanged();
  void packageOperationStarted( PackageOperation operation, const QString &name);
  void packageOperationFinished(PackageOperation operation, const QString &name, const QString &version);
  void repositoryRefreshed();

private:
  explicit Chum(QObject *parent = nullptr);

  QString packageId(const QString &pkg_id);

  void getUpdatesFinished();
  void refreshPackages();
  void refreshPackagesFinished();
  void refreshDetails();
  void refreshInstalledVersion();

  void startOperation(PackageKit::Transaction *pktr, const QString &pkg_id);

private:
  bool          m_busy{false};
  QString       m_status;
  quint32       m_updates_count{0};

  QHash<QString, ChumPackage*> m_packages;
  QSet<QString>                m_packages_last_refresh;

  // static
  static Chum*         s_instance;
  static const QString s_repo_name;
};
