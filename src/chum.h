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

  // repo operations
  bool refreshingRepo() const {
    return m_refreshing_repo;
  }

  quint32 updatesCount() const {
    return m_updates_count;
  }

  // static public methods
  static Chum* instance();
  static bool isChumPackage(const QString &id);

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
  explicit Chum(QObject *parent = nullptr);

  void startOperation(PackageKit::Transaction *pktr, const QString &pkg_id);

private:
  bool          m_refreshing_repo{false};
  quint32       m_updates_count{0};

  static Chum* s_instance;
  static const QString s_repoName;
};
