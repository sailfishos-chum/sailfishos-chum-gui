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

  Q_PROPERTY(bool    filterInstalledOnly READ filterInstalledOnly WRITE setFilterInstalledOnly NOTIFY filterInstalledOnlyChanged)
  Q_PROPERTY(bool    filterUpdatesOnly READ filterUpdatesOnly WRITE setFilterUpdatesOnly NOTIFY filterUpdatesOnlyChanged)
  Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)

public:
  explicit ChumPackagesModel(QObject *parent = nullptr);

  bool filterInstalledOnly() const { return m_filter_installed_only; }
  bool filterUpdatesOnly() const { return m_filter_updates_only; }
  QString search() const { return m_search; }

  void setFilterInstalledOnly(bool filter);
  void setFilterUpdatesOnly(bool filter);
  void setSearch(QString search);

  Q_INVOKABLE void reset();

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void classBegin() override {}
  void componentComplete() override { m_postpone_loading=false; reset(); }

signals:
  void filterUpdatesOnlyChanged();
  void filterInstalledOnlyChanged();
  void searchChanged();

private:
  void updatePackage(QString packageId, ChumPackage::Role role);

private:
  QList<QString> m_packages;
  bool           m_postpone_loading{true};

  bool m_filter_installed_only{false};
  bool m_filter_updates_only{false};
  QString m_search;
};
