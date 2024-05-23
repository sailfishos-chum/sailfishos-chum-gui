#include "ssu.h"

#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDebug>

#if defined(SAILFISHOS_TARGET_VERSION) && SAILFISHOS_TARGET_VERSION >= 40600
#define RELEASE_TAG "%(releaseMajorMinor)"
#else
#define RELEASE_TAG "%(release)"
#endif

static QString s_repo_regular(
        QStringLiteral("https://repo.sailfishos.org/obs/sailfishos:/chum/" RELEASE_TAG "_%(arch)/"));
static QString s_repo_regular_alias(
        QStringLiteral("sailfishos-chum"));
static QString s_repo_regular_prefix(
        QStringLiteral("https://repo.sailfishos.org/obs/sailfishos:/chum/"));
static QString s_repo_testing(
        QStringLiteral("https://repo.sailfishos.org/obs/sailfishos:/chum:/testing/" RELEASE_TAG "_%(arch)/"));
static QString s_repo_testing_alias(
        QStringLiteral("sailfishos-chum-testing"));
static QString s_repo_testing_prefix(
        QStringLiteral("https://repo.sailfishos.org/obs/sailfishos:/chum:/testing/"));

Ssu::Ssu(QObject *parent) :
    QDBusAbstractInterface(
        QStringLiteral("org.nemo.ssu"),
        QStringLiteral("/org/nemo/ssu"),
        "org.nemo.ssu",
        QDBusConnection::systemBus(),
        parent )
{
}

void Ssu::loadRepos() {
    // rnd set to false to have repos with the version instead of "latest"
    QDBusPendingCall pcall = asyncCall(QStringLiteral("listRepos"), false);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
                     this, &Ssu::onListFinished);
}

void Ssu::onListFinished(QDBusPendingCallWatcher *call) {
    m_repos.clear();

    // load repos from DBus call
    QVariantList vlist = call->reply().arguments();
    if (vlist.length() != 1) {
        qWarning() << "Unexpected reply from SSU:" << call->reply();
        m_manage_repo = false;
        emit updated();
        return;
    }
    const QDBusArgument &arg = vlist[0].value<QDBusArgument>();
    arg.beginArray();
    while (!arg.atEnd()) {
        arg.beginStructure();
        QString name;
        QString url;
        QVariantMap pars;
        arg >> name >> url >> pars;
        arg.endStructure();
        //qDebug() << name << url << pars;
        std::pair<QString,QString> r(name, url);
        m_repos.append(r);
    }
    arg.endArray();

    // count chum repos
    int count = 0;
    bool has_reg = false;
    bool has_wrong_alias = false;
    QString repo_name;
    for (const auto &u: m_repos)
        if (u.second.startsWith(s_repo_regular_prefix) && u.first == s_repo_regular_alias) {
            ++count;
            has_reg = true;
            repo_name = u.first;
        } else if (u.second.startsWith(s_repo_testing_prefix) && u.first == s_repo_testing_alias) {
            ++count;
            repo_name = u.first;
        } else if (u.first == s_repo_regular_alias || u.first == s_repo_testing_alias) {
            has_wrong_alias = true;
        }

    if (count > 1) {
        qWarning() << "More than one Chum repository defined in SSU - skipping management of repositories";
        m_manage_repo = false;
        emit updated();
        return;
    }

    if (has_wrong_alias) {
        qWarning() << "Some of the expected Chum aliases taken by wrong repository - skipping management of repositories";
        m_manage_repo = false;
        emit updated();
        return;
    }

    if (has_reg) m_repo_testing = false;
    else if (count == 1) m_repo_testing = true;

    if (count == 1) m_repo_name = repo_name;
    else m_repo_name.clear();

    m_manage_repo = true;

    // Check if addition worked out as expected
    if (!m_added_repo_name.isEmpty() && m_added_repo_name != m_repo_name) {
        qWarning() << "Failed to add new repository with alias " << m_added_repo_name;
        qWarning() << "Check whether this alias has been defined by SSU in enabled or disabled repositories";
        qWarning() << "Remove such repository definition from SSU if it is there";
        m_manage_repo = false;
        emit updated();
        return;
    }
    m_added_repo_name.clear();

    emit updated();
}

void Ssu::setRepo(const QString &version, bool testing) {
    if (!m_manage_repo) {
        qWarning() << "Cannot set repository via SSU - management disabled";
        return;
    }

    if (repoAvailable()) {
        // remove current repository
        callWithArgumentList(QDBus::BlockWithGui, QStringLiteral("modifyRepo"),
                             QVariantList{0, m_repo_name});
    }

    // find new repo name
    QString rname = testing ? s_repo_testing_alias : s_repo_regular_alias;
    QString url = testing ? s_repo_testing : s_repo_regular;

    if (!version.isEmpty()) {
        url = url.replace(QLatin1String(RELEASE_TAG), version);
    }

    if (rname != m_repo_name) {
        // check if proposed name is taken
        for (const auto &u: m_repos) {
            if (u.first == rname) {
                qWarning() << "Expected free repository alias already taken - skipping management of repositories";
                m_manage_repo = false;
                emit updated();
                return;
            }
        }
    }

    m_repo_name.clear();

    // add repo
    callWithArgumentList(QDBus::BlockWithGui, QStringLiteral("addRepo"),
                         QVariantList{rname, url});
    call(QDBus::BlockWithGui, QStringLiteral("updateRepos"));
    m_added_repo_name = rname;

    // refresh list
    loadRepos();
}
