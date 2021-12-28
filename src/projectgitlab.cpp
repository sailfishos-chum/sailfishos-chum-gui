#include "projectgitlab.h"
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

static QString reqAuth{QStringLiteral("Bearer " GITLAB_TOKEN)};
static QString reqUrl{QStringLiteral("https://gitlab.com/api/graphql")};

//////////////////////////////////////////////////////
/// helper functions

static QString getName(const QVariant &v) {
  QVariantMap m = v.toMap();
  QString login = m[QLatin1String("username")].toString();
  QString name = m[QLatin1String("name")].toString();
  if (login.isEmpty() || name==login) return name;
  if (name.isEmpty()) return login;
  return QStringLiteral("%1 (%2)").arg(name, login);
}

static QNetworkReply* sendQuery(const QString &query) {
  QNetworkRequest request;
  request.setUrl(reqUrl);
  request.setRawHeader("Content-Type", "application/json");
  request.setRawHeader("Authorization", reqAuth.toLocal8Bit());
  return nMng->post(request, query.toLocal8Bit());
}

static bool parseUrl(const QString &u, QString &path) {
  QUrl url(u);
  QString host = url.host();
  QString p = url.path();
  if (p.startsWith('/')) p = p.mid(1);
  if (p.endsWith('/')) p.chop(1);

  if (host != QStringLiteral("gitlab.com"))
    return false;

  path = p;
  return true;
}

static QString parseDate(QString txt, bool short_format=false) {
  QDateTime dt = QDateTime::fromString(txt, Qt::ISODate);
  return QLocale::system().toString(dt.toLocalTime().date(),
                                    short_format ? QLocale::ShortFormat :
                                                   QLocale::LongFormat);
}

//static QString parseDateTime(QString txt) {
//  QDateTime dt = QDateTime::fromString(txt, Qt::ISODate);
//  return QLocale::system().toString(dt.toLocalTime(), QLocale::LongFormat);
//}

//////////////////////////////////////////////////////
/// ProjectGitLab

ProjectGitLab::ProjectGitLab(const QString &url, ChumPackage *package) : ProjectAbstract(package) {
  bool ok = parseUrl(url, m_path);
  if (!ok) {
    qWarning() << "Shouldn't happen: ProjectGitLab initialized with incorrect service" << url;
    return;
  }

  // url is not set as it can be different homepage that is retrieved from query
  m_package->setUrlIssues(QStringLiteral("https://gitlab.com/%1/-/issues").arg(m_path));

  // fetch information from GitLab
  fetchRepoInfo();
}

// static
bool ProjectGitLab::isProject(const QString &url) {
  QString p;
  return parseUrl(url, p);
}


void ProjectGitLab::fetchRepoInfo() {
  QString query = QStringLiteral(R"(
{
"query": "
query {
  project(fullPath: \"%1\") {
    forksCount
    openIssuesCount
    mergeRequests(state: opened) {
      count
    }
    releases {
      count
    }
    starCount
  }
}"
}
)").arg(m_path);
  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, [this, reply](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch repository data for GitLab" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QJsonObject r{QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject()};

    int vi;

    vi = r.value("starCount").toInt(-1);
    if (vi>=0) m_package->setStarsCount(vi);

    vi = r.value("forksCount").toInt(0);
    if (vi>=0) m_package->setForksCount(vi);

    vi = r.value("openIssuesCount").toInt(0);
    if (vi>=0) m_package->setIssuesCount(vi);

    vi = r.value("releases").toObject().value("count").toInt();
    if (vi>=0) m_package->setReleasesCount(vi);

    reply->deleteLater();
  });
}


void ProjectGitLab::issue(const QString &issue_id, LoadableObject *value) {
  if (value->ready() && value->valueId()==issue_id)
    return; // value already corresponds to that release
  value->reset(issue_id);

  QString query = QStringLiteral(R"(
{
"query": "
query {
  project(fullPath: \"%1\") {
    issue(iid: \"%2\") {
      iid
      title
      author {
        name
        username
      }
      descriptionHtml
      createdAt
      updatedAt
      notes {
        nodes {
          author {
            name
            username
          }
          bodyHtml
          createdAt
          updatedAt
        }
      }
    }
  }
}"
}
)").arg(m_path, issue_id);

  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, [this, issue_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch issue for" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantMap r = QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject().
          value("issue").toObject().toVariantMap();

    QVariantMap result;
    result["id"] = r.value("iid");
    result["number"] = r.value("iid");
    result["title"] = r.value("title");
    QVariantList clist = r.value("notes").toMap().value("nodes").toList();
    QVariantList result_list;
    result["commentsCount"] = clist.size();
    QVariantMap m;
    m["author"] = getName(r.value("author"));
    m["created"] = parseDate(r.value("createdAt").toString(), true);
    m["updated"] = parseDate(r.value("updatedAt").toString(), true);
    m["body"] = r.value("descriptionHtml").toString();
    result_list.append(m);
    // iterate in reverse as gitlab returns notes in reverse order
    for (auto e=clist.rbegin(); e!=clist.rend(); ++e) {
      QVariantMap element = e->toMap();
      m.clear();
      m["author"] = getName(element.value("author"));
      m["created"] = parseDate(element.value("createdAt").toString(), true);
      m["updated"] = parseDate(element.value("updatedAt").toString(), true);
      m["body"] = element.value("bodyHtml").toString();
      result_list.append(m);
    }

    result["discussion"] = result_list;
    value->setValue(issue_id, result);
    reply->deleteLater();
  });
}


void ProjectGitLab::issues(LoadableObject *value) {
  const QString issues_id{QStringLiteral("issues")};
  value->reset(issues_id);

  QString query = QStringLiteral(R"(
{
"query": "
query {
  project(fullPath: \"%1\") {
    issues(state: opened, sort: UPDATED_DESC) {
      nodes {
        iid
        title
        author {
          name
          username
        }
        createdAt
        updatedAt
        notes {
          nodes {
            createdAt
          }
        }
      }
    }
  }
}"
}
)").arg(m_path);
  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, [this, issues_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch issues for" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject().
          value("issues").toObject().value("nodes").toArray().toVariantList();

    QVariantList rlist;
    for (auto e: r) {
      QVariantMap element = e.toMap();
      QVariantMap m;
      m["id"] = element.value("iid");
      m["author"] = getName(element.value("author"));
      m["commentsCount"] = element.value("notes").toMap().value("nodes").toList().size();
      m["number"] = element.value("iid");
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


void ProjectGitLab::release(const QString &release_id, LoadableObject *value) {
  if (value->ready() && value->valueId()==release_id)
    return; // value already corresponds to that release
  value->reset(release_id);

  QString query = QStringLiteral(R"(
{
"query": "
query {
  project(fullPath: \"%1\") {
    release(tagName: \"%2\") {
      name
      createdAt
      descriptionHtml
    }
  }
}"
}
)").arg(m_path, release_id);

  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, [this, release_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch release for" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantMap r = QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject().
          value("release").toObject().toVariantMap();

    QVariantMap result;
    result["name"] = r.value("name");
    result["description"] = r.value("descriptionHtml");
    result["datetime"] = parseDate(r.value("createdAt").toString());

    value->setValue(release_id, result);
    reply->deleteLater();
  });
}


void ProjectGitLab::releases(LoadableObject *value) {
  const QString releases_id{QStringLiteral("releases")};
  value->reset(releases_id);

  QString query = QStringLiteral(R"(
{
"query": "
query {
  project(fullPath: \"%1\") {
    releases {
      nodes {
        name
        tagName
        createdAt
      }
    }
  }
}"
}
)").arg(m_path);
  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, [this, releases_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch releases for" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject().
          value("releases").toObject().value("nodes").toArray().toVariantList();

    QVariantList rlist;
    for (auto e: r) {
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
