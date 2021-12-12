#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QSet>

#include "chumpackage.h"

class ChumPackagesModel
  : public QAbstractListModel
  , public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)

public:
  explicit ChumPackagesModel(QObject *parent = nullptr);

  QString search() const { return m_search; }

  void setSearch(QString search);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void classBegin() override {}
  void componentComplete() override { reset(); }

signals:
  void searchChanged();

private:
  void reset();
  void updatePackage(QString packageId, ChumPackage::Role role);

private:
  QList<QString> m_packages;

  QString m_search;
};
