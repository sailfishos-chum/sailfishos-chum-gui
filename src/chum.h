#pragma once

#include <QObject>

class Chum : public QObject {
  Q_OBJECT
  Q_PROPERTY(quint32 updatesCount READ updatesCount NOTIFY updatesCountChanged)

public:

  static const QString repoName;

  explicit Chum(QObject *parent = nullptr);

  quint32 updatesCount() const {
    return m_updates_count;
  }

  static bool isChumPackage(const QString &id);

public slots:
  void getUpdates();

signals:
  void updatesCountChanged();

private:
  quint32       m_updates_count{0};
};
