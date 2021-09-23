#pragma once

#include <QObject>

class Chum : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool    refreshingRepo READ refreshingRepo() NOTIFY refreshingRepoChanged)
  Q_PROPERTY(quint32 updatesCount   READ updatesCount     NOTIFY updatesCountChanged)

public:
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

signals:
  void refreshingRepoChanged();
  void updatesCountChanged();

private:
  bool          m_refreshing_repo{false};
  quint32       m_updates_count{0};
};
