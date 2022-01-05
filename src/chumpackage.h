#pragma once

#include <QObject>
#include <PackageKit/Details>

#include "loadableobject.h"
#include "projectabstract.h"

class ChumPackage : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString id READ id NOTIFY idChanged)

    Q_PROPERTY(QString    availableVersion READ availableVersion NOTIFY updated)
    Q_PROPERTY(QStringList categories READ categories   NOTIFY updated)
    Q_PROPERTY(QString    description READ description  NOTIFY updated)
    Q_PROPERTY(QString    developer   READ developer    NOTIFY updated)
    Q_PROPERTY(QString    donation    READ donation     NOTIFY updated)
    Q_PROPERTY(int        forksCount  READ forksCount   NOTIFY updated)
    Q_PROPERTY(QString    icon        READ icon         NOTIFY updated)
    Q_PROPERTY(bool       installed   READ installed    NOTIFY updated)
    Q_PROPERTY(QString    installedVersion READ installedVersion NOTIFY updated)
    Q_PROPERTY(int        issuesCount READ issuesCount  NOTIFY updated)
    Q_PROPERTY(QString    license     READ license      NOTIFY updated)
    Q_PROPERTY(QString    name        READ name         NOTIFY updated)
    Q_PROPERTY(QString    packageName READ packageName  NOTIFY updated)
    Q_PROPERTY(int        releasesCount READ releasesCount  NOTIFY updated)
    Q_PROPERTY(QString    repo        READ repo         NOTIFY updated)
    Q_PROPERTY(QStringList screenshots READ screenshots NOTIFY updated)
    Q_PROPERTY(qulonglong size        READ size         NOTIFY updated)
    Q_PROPERTY(int        starsCount  READ starsCount   NOTIFY updated)
    Q_PROPERTY(QString    summary     READ summary      NOTIFY updated)
    Q_PROPERTY(PackageType type       READ type         NOTIFY updated)
    Q_PROPERTY(bool       updateAvailable READ updateAvailable NOTIFY updated)
    Q_PROPERTY(QString    url         READ url          NOTIFY updated)
    Q_PROPERTY(QString    urlForum    READ urlForum     NOTIFY updated)
    Q_PROPERTY(QString    urlIssues   READ urlIssues    NOTIFY updated)

public:
    enum Role {
        PackageIdRole = Qt::UserRole + 1,
        PackageAvailableVersionRole,
        PackageCategoriesRole,
        PackageDescriptionRole,
        PackageDeveloperRole,
        PackageIconRole,
        PackageInstalledRole,
        PackageInstalledVersionRole,
        PackageNameRole,
        PackageStarsCountRole,
        PackageSummaryRole,
        PackageTypeRole,
        PackageUpdateAvailableRole,

        PackageOtherRole,
        PackageRefreshRole // used for updates of many parameters
    };

    enum PackageType {
        PackageApplicationDesktop,
        PackageApplicationConsole,
        PackageGeneric
    };
    Q_ENUM(PackageType)

    ChumPackage(QObject *parent = nullptr);
    ChumPackage(const QString &id, QObject *parent = nullptr);

    Q_INVOKABLE LoadableObject* issue(const QString &id);
    Q_INVOKABLE LoadableObject* issues();
    Q_INVOKABLE LoadableObject* release(const QString &id);
    Q_INVOKABLE LoadableObject* releases();

    QString id() const { return m_id; }
    QString pkidLatest() const { return m_pkid_latest; }
    QString pkidInstalled() const { return m_pkid_installed; }
    bool detailsNeedsUpdate() const { return m_details_update; }

    QString availableVersion() const { return m_available_version; }
    QStringList categories() const { return m_categories; }
    QString description() const { return m_description; }
    QString developer() const;
    QString donation() const { return m_donation; }
    int     forksCount() const { return m_forks_count; }
    QString icon() const { return m_icon; }
    bool    installed() const;
    QString installedVersion() const { return m_installed_version; }
    int     issuesCount() const { return m_issues_count; }
    QString license() const { return m_license; }
    QString name() const { return m_name; }
    QString packageName() const { return m_package_name; }
    QString repo() const { return m_repo_url; }
    int     releasesCount() const { return m_releases_count; }
    QStringList screenshots() const { return m_screenshots; }
    qulonglong size() const { return m_size; }
    int     starsCount() const { return m_stars_count; }
    QString summary() const { return m_summary; }
    PackageType type() const { return m_type; }
    bool    updateAvailable() const { return m_update_available; }
    QString url() const { return m_url; }
    QString urlForum() const { return m_url_forum; }
    QString urlIssues() const { return m_url_issues; }

    void setPkidLatest(const QString &pkid);
    void setPkidInstalled(const QString &pkid);
    void setUpdateAvailable(bool up);
    void setDetails(const PackageKit::Details &v);
    void clearInstalled();

    void setDeveloperLogin(const QString &login);
    void setDeveloperName(const QString &name);
    void setForksCount(int count);
    void setIssuesCount(int count);
    void setReleasesCount(int count);
    void setStarsCount(int count);
    void setUrl(const QString &url);
    void setUrlForum(const QString &url);
    void setUrlIssues(const QString &url);

signals:
    void idChanged();
    void updated(QString packageId, ChumPackage::Role role);
    void updateAvailableChanged();

private:
    void setInstalledVersion(const QString &v);

private:
    ProjectAbstract *m_project{nullptr};
    LoadableObject  *m_issue_info{nullptr};
    LoadableObject  *m_issues{nullptr};
    LoadableObject  *m_release_info{nullptr};
    LoadableObject  *m_releases{nullptr};

    QString     m_id; // ID of the package as used in Chum
    QString     m_pkid_latest; // Package ID as set by PackageKit
    QString     m_pkid_installed; // Package ID as set by PackageKit
    QString     m_installed_version;
    bool        m_update_available{false};
    bool        m_details_update{false};

    QString     m_available_version;
    QStringList m_categories;
    QString     m_description;
    QString     m_developer_login;
    QString     m_developer_name;
    QString     m_donation;
    int         m_forks_count{-1};
    QString     m_icon;
    int         m_issues_count{-1};
    QString     m_license;
    QString     m_name;
    QString     m_package_name;
    int         m_releases_count{-1};
    QString     m_repo_type;
    QString     m_repo_url;
    QStringList m_screenshots;
    qulonglong  m_size{0};
    int         m_stars_count{-1};
    QString     m_summary;
    PackageType m_type;
    QString     m_url;
    QString     m_url_forum;
    QString     m_url_issues;
};
