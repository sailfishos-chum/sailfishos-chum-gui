#include "projectgithub.h"
#include "chumpackage.h"
#include "main.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocale>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

static QString reqAuth{QStringLiteral("bearer " GITHUB_TOKEN)};
static QString reqUrl{QStringLiteral("https://api.github.com/graphql")};

//////////////////////////////////////////////////////
/// helper functions

static QString getName(const QVariant &v) {
    QVariantMap m = v.toMap();
    QString login = m[QLatin1String("login")].toString();
    QString name = m[QLatin1String("name")].toString();
    if (login.isEmpty()) return name;
    if (name.isEmpty()) return login;
    return QStringLiteral("%1 (%2)").arg(name, login);
}

static QNetworkReply* sendQuery(const QString &query) {
    QNetworkRequest request;
    request.setUrl(reqUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", reqAuth.toLocal8Bit());
    return nMng->post(request, query.toLocal8Bit());
}

static bool parseUrl(const QString &u, QString &org, QString &repo) {
    QUrl url(u);
    QString host = url.host();
    QStringList path = url.path().split('/', QString::SkipEmptyParts);

    if (host != QStringLiteral("github.com") || path.length() != 2)
        return false;

    org = path[0];
    repo = path[1];
    return true;
}

//////////////////////////////////////////////////////
/// ProjectGitHub

ProjectGitHub::ProjectGitHub(const QString &url, ChumPackage *package) : ProjectAbstract(package) {
    bool ok = parseUrl(url, m_org, m_repo);
    if (!ok) {
        qWarning() << "Shouldn't happen: ProjectGitHub initialized with incorrect service" << url;
        return;
    }

    // settings that can be set from knowing repo url
    m_package->setDeveloperLogin(m_org);
    // url is not set as it can be different homepage that is retrieved from query
    m_package->setUrlIssues(QStringLiteral("https://github.com/%1/%2/issues").arg(m_org, m_repo));
    // urldiscussion is not set as it is not used by all projects

    // fetch information from GitHub
    fetchRepoInfo();
}

// static
bool ProjectGitHub::isProject(const QString &url) {
    QString o, p;
    return parseUrl(url, o, p);
}


void ProjectGitHub::fetchRepoInfo() {
    QString query = QStringLiteral(R"(
{
"query": "
query {
  repository(owner: \"%1\", name:\"%2\") {
    owner {
      ... on User {
        login
        name
      }
      ... on Organization {
        login
        name
      }
    }
    stargazerCount
    homepageUrl
    forks {
      totalCount
    }
    issues(states: OPEN) {
      totalCount
    }
    releases {
      totalCount
    }
    discussions {
      totalCount
    }
    pullRequests(states:OPEN) {
      totalCount

    }
  }
}"
}
)").arg(m_org, m_repo);
    query = query.replace('\n', ' ');

    QNetworkReply *reply = sendQuery(query);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to fetch repository data for" << this->m_org << this->m_repo;
            qWarning() << "Error: " << reply->errorString();
        }

        QJsonObject r{QJsonDocument::fromJson(reply->readAll()).object().
                    value("data").toObject().value("repository").toObject()};

        QString v;
        int vi;

        // set login before name
        v = r.value("owner").toObject().value("login").toString();
        if (!v.isEmpty()) m_package->setDeveloperLogin(v);

        v = r.value("owner").toObject().value("name").toString();
        if (!v.isEmpty()) m_package->setDeveloperName(v);

        vi = r.value("stargazerCount").toInt(-1);
        if (vi>=0) m_package->setStarsCount(vi);

        v = r.value("homepageUrl").toString();
        if (!v.isEmpty()) m_package->setUrl(v);
        else m_package->setUrl(QStringLiteral("https://github.com/%1/%2").arg(m_org, m_repo));

        vi = r.value("forks").toObject().value("totalCount").toInt(-1);
        if (vi>=0) m_package->setForksCount(vi);

        vi = r.value("issues").toObject().value("totalCount").toInt(-1);
        if (vi>=0) m_package->setIssuesCount(vi);

        vi = r.value("releases").toObject().value("totalCount").toInt(-1);
        if (vi>=0) m_package->setReleasesCount(vi);

        vi = r.value("discussions").toObject().value("totalCount").toInt();
        if (vi>=0)
            m_package->setUrlForum(QStringLiteral("https://github.com/%1/%2/discussions").arg(m_org, m_repo));

        reply->deleteLater();
    });
}


void ProjectGitHub::issue(const QString &issue_id, LoadableObject *value) {
    if (value->ready() && value->valueId()==issue_id)
        return; // value already corresponds to that release
    value->reset(issue_id);

    QString query = QStringLiteral(R"(
{
"query": "
query {
  repository(owner: \"%1\", name:\"%2\") {
    issue(number: %3) {
      number
      title
      author {
        ... on User {
          login
          name
        }
        ... on Organization {
          login
          name
        }
      }
      bodyHTML
      createdAt
      updatedAt
      comments(last:100) {
        totalCount
        nodes {
          author {
            ... on User {
              login
              name
            }
            ... on Organization {
              login
              name
            }
          }
          bodyHTML
          createdAt
          updatedAt
        }
      }
    }
  }
}"
}
)").arg(m_org, m_repo, issue_id);

    query = query.replace('\n', ' ');

    QNetworkReply *reply = sendQuery(query);
    connect(reply, &QNetworkReply::finished, this, [this, issue_id, reply, value](){
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to fetch issue for" << this->m_org << this->m_repo;
            qWarning() << "Error: " << reply->errorString();
        }

        QByteArray data = reply->readAll();
        QVariantMap r = QJsonDocument::fromJson(data).object().
                value("data").toObject().value("repository").toObject().
                value("issue").toObject().toVariantMap();

        QVariantMap result;
        result["id"] = r.value("number");
        result["commentsCount"] = r.value("comments").toMap().value("totalCount").toInt();
        result["number"] = r.value("number");
        result["title"] = r.value("title");
        QVariantList clist = r.value("comments").toMap().value("nodes").toList();
        QVariantList result_list;
        QVariantMap m;
        m["author"] = getName(r.value("author"));
        m["created"] = parseDate(r.value("createdAt").toString(), true);
        m["updated"] = parseDate(r.value("updatedAt").toString(), true);
        m["body"] = r.value("bodyHTML").toString();
        result_list.append(m);
        for (const auto &e: clist) {
            QVariantMap element = e.toMap();
            m.clear();
            m["author"] = getName(element.value("author"));
            m["created"] = parseDate(element.value("createdAt").toString(), true);
            m["updated"] = parseDate(element.value("updatedAt").toString(), true);
            m["body"] = element.value("bodyHTML").toString();
            result_list.append(m);
        }

        result["discussion"] = result_list;
        value->setValue(issue_id, result);
        reply->deleteLater();
    });
}


void ProjectGitHub::issues(LoadableObject *value) {
    const QString issues_id{QStringLiteral("issues")};
    value->reset(issues_id);

    QString query = QStringLiteral(R"(
{
"query": "
query {
  repository(owner: \"%1\", name:\"%2\") {
    issues(first: 100, states: OPEN, orderBy: {field: UPDATED_AT, direction: DESC}) {
      nodes {
        number
        title
        author {
          ... on User {
            login
            name
          }
          ... on Organization {
            login
            name
          }
        }
        createdAt
        updatedAt
        comments {
          totalCount
        }
      }
    }
  }
}"
}
)").arg(m_org, m_repo);
    query = query.replace('\n', ' ');

    QNetworkReply *reply = sendQuery(query);
    connect(reply, &QNetworkReply::finished, this, [this, issues_id, reply, value](){
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to fetch issues for" << this->m_org << this->m_repo;
            qWarning() << "Error: " << reply->errorString();
        }

        QByteArray data = reply->readAll();
        QVariantList r = QJsonDocument::fromJson(data).object().
                value("data").toObject().value("repository").toObject().
                value("issues").toObject().value("nodes").toArray().toVariantList();

        QVariantList rlist;
        for (const auto &e: r) {
            QVariantMap element = e.toMap();
            QVariantMap m;
            m["id"] = element.value("number");
            m["author"] = getName(element.value("author"));
            m["commentsCount"] = element.value("comments").toMap().value("totalCount").toInt();
            m["number"] = element.value("number");
            m["title"] = element.value("title");
            m["created"] = parseDate(element.value("createdAt").toString(), true);
            m["updated"] = parseDate(element.value("updatedAt").toString(), true);
            rlist.append(m);
        }

        QVariantMap result;
        result["issues"] = rlist;
        value->setValue(issues_id, result);

        reply->deleteLater();
    });
}


void ProjectGitHub::release(const QString &release_id, LoadableObject *value) {
    if (value->ready() && value->valueId()==release_id)
        return; // value already corresponds to that release
    value->reset(release_id);

    QString query = QStringLiteral(R"(
{
"query": "
query {
  repository(owner: \"%1\", name:\"%2\") {
    release(tagName: \"%3\") {
      name
      createdAt
      descriptionHTML
    }
  }
}"
}
)").arg(m_org, m_repo, release_id);

    query = query.replace('\n', ' ');

    QNetworkReply *reply = sendQuery(query);
    connect(reply, &QNetworkReply::finished, this, [this, release_id, reply, value](){
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to fetch release for" << this->m_org << this->m_repo;
            qWarning() << "Error: " << reply->errorString();
        }

        QByteArray data = reply->readAll();
        QVariantMap r = QJsonDocument::fromJson(data).object().
                value("data").toObject().value("repository").toObject().
                value("release").toObject().toVariantMap();

        QVariantMap result;
        result["name"] = r.value("name");
        result["description"] = r.value("descriptionHTML");
        result["datetime"] = parseDate(r.value("createdAt").toString());

        value->setValue(release_id, result);
        reply->deleteLater();
    });
}


void ProjectGitHub::releases(LoadableObject *value) {
    const QString releases_id{QStringLiteral("releases")};
    value->reset(releases_id);

    QString query = QStringLiteral(R"(
{
"query": "
query {
  repository(owner: \"%1\", name:\"%2\") {
    releases(first: 30, orderBy: {field: CREATED_AT, direction: DESC}) {
      totalCount
      nodes {
        createdAt
        name
        tagName
      }
    }
  }
}"
}
)").arg(m_org, m_repo);
    query = query.replace('\n', ' ');

    QNetworkReply *reply = sendQuery(query);
    connect(reply, &QNetworkReply::finished, this, [this, releases_id, reply, value](){
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to fetch releases for" << this->m_org << this->m_repo;
            qWarning() << "Error: " << reply->errorString();
        }

        QByteArray data = reply->readAll();
        QVariantList r = QJsonDocument::fromJson(data).object().
                value("data").toObject().value("repository").toObject().
                value("releases").toObject().value("nodes").toArray().toVariantList();

        QVariantList rlist;
        for (const auto &e: r) {
            QVariantMap element = e.toMap();
            QVariantMap m;
            m["id"] = element.value("tagName");
            m["name"] = element.value("name");
            m["datetime"] = parseDate(element.value("createdAt").toString());
            rlist.append(m);
        }

        QVariantMap result;
        result["releases"] = rlist;
        value->setValue(releases_id, result);

        reply->deleteLater();
    });
}
