#pragma once

#include <QObject>

namespace PackageKit {
class Transaction;
}

class Chum : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool    refreshingRepo READ refreshingRepo() NOTIFY refreshingRepoChanged)
  Q_PROPERTY(quint32 updatesCount   READ updatesCount     NOTIFY updatesCountChanged)

public:
  enum PackageOperation {
    PackageUnknownOperation,
    PackageInstallation,
    PackageUpdate,
  };
  Q_ENUM(PackageOperation)

  static const QString repoName;

  static bool isChumPackage(const QString &id);

  explicit Chum(QObject *parent = nullptr);

  bool refreshingRepo() const {
    return m_refreshing_repo;
  }

  quint32 updatesCount() const {
    return m_updates_count;
  }

public slots:
  void getUpdates();
  void refreshRepo(bool force = false);
  void installPackage(const QString &id);
  void updatePackage(const QString &id);

signals:
  void refreshingRepoChanged();
  void updatesCountChanged();
  void packageOperationStarted( PackageOperation operation, const QString &name);
  void packageOperationFinished(PackageOperation operation, const QString &name, const QString &version);

private:
  void startOperation(PackageKit::Transaction *pktr, const QString &pkg_id);

private:
  bool          m_refreshing_repo{false};
  quint32       m_updates_count{0};
};
