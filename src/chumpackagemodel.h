#pragma once

#include <QAbstractListModel>

class ChumPackageModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Role {
    PackageIdRole,
    PackageNameRole,
    PackageVersionRole,
  };

  explicit ChumPackageModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

public slots:
  void reset();

private:
  struct Package {
    QString id;
    QString name;
    QString version;
  };
  QVector<Package> m_data;
};
