#pragma once

#include <QAbstractListModel>
#include <QQmlParserStatus>

class ChumAbstractPackageModel
  : public QAbstractListModel
  , public QQmlParserStatus
{
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

public:
  enum Role {
    PackageIdRole,
    PackageNameRole,
    PackageVersionRole,
    PackageSummaryRole,
  };

  explicit ChumAbstractPackageModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void classBegin() override {}
  void componentComplete() override {
    reset();
  }

public slots:
  virtual void reset() = 0;

protected slots:
  void insertPackage(int info, const QString &packageID, const QString &summary);

private:
  struct Package {
    QString id;
    QString name;
    QString version;
    QString summary;
  };
  QVector<Package> m_data;
};
