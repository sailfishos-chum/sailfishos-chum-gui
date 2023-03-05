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

    Q_PROPERTY(bool    filterApplicationsOnly READ filterApplicationsOnly WRITE setFilterApplicationsOnly NOTIFY filterApplicationsOnlyChanged)
    Q_PROPERTY(bool    filterInstalledOnly READ filterInstalledOnly WRITE setFilterInstalledOnly NOTIFY filterInstalledOnlyChanged)
    Q_PROPERTY(bool    filterUpdatesOnly READ filterUpdatesOnly WRITE setFilterUpdatesOnly NOTIFY filterUpdatesOnlyChanged)
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)
    Q_PROPERTY(QString showCategory READ showCategory WRITE setShowCategory NOTIFY showCategoryChanged)

public:
    explicit ChumPackagesModel(QObject *parent = nullptr);

    bool filterApplicationsOnly() const { return m_filter_applications_only; }
    bool filterInstalledOnly() const { return m_filter_installed_only; }
    bool filterUpdatesOnly() const { return m_filter_updates_only; }
    QString search() const { return m_search; }
    QString showCategory() const { return m_show_category.toList().join(QChar(';')); }

    void setFilterApplicationsOnly(bool filter);
    void setFilterInstalledOnly(bool filter);
    void setFilterUpdatesOnly(bool filter);
    void setSearch(QString search);
    void setShowCategory(QString category);

    Q_INVOKABLE void reset();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void classBegin() override {}
    void componentComplete() override { m_postpone_loading=false; reset(); }

signals:
    void filterApplicationsOnlyChanged();
    void filterInstalledOnlyChanged();
    void filterUpdatesOnlyChanged();
    void searchChanged();
    void showCategoryChanged();
    void busyChanged();

private:
    void updatePackage(QString packageId, ChumPackage::Role role);

private:
    QList<QString> m_packages;
    bool           m_postpone_loading{true};

    bool m_filter_applications_only{false};
    bool m_filter_installed_only{false};
    bool m_filter_updates_only{false};
    QString m_search;
    QSet<QString> m_show_category;
};
