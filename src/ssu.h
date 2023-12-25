#ifndef SSU_H
#define SSU_H

#include <QDBusAbstractInterface>
#include <QDBusPendingCallWatcher>
#include <QList>
#include <utility>

class Ssu : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    explicit Ssu(QObject *parent = nullptr);

    bool manageRepo() const { return m_manage_repo; }
    bool repoAvailable() const { return m_manage_repo && !m_repo_name.isEmpty(); }
    bool repoTesting() const { return m_manage_repo && m_repo_testing; }
    QString repoName() const { return m_manage_repo ? m_repo_name : QString{}; }
    QString deviceVersion() const {return m_device_version;}

    void loadRepos();
    void setRepo(const QString &version=QString(), bool testing=false);

signals:
    void updated();

private:
    void onListFinished(QDBusPendingCallWatcher *call);

private:
    bool m_manage_repo{false};
    bool m_repo_testing{false};
    QString m_repo_name;
    QString m_device_version;

    QList< std::pair<QString,QString> > m_repos;
    QString m_added_repo_name;
};

#endif // SSU_H
