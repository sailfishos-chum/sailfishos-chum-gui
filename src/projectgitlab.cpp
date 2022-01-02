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

QMap<QString, QString> ProjectGitLab::s_sites;

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

static void parseUrl(const QString &u, QString &h, QString &path) {
  QUrl url(u);
  h = url.host();
  QString p = url.path();
  if (p.startsWith('/')) p = p.mid(1);
  if (p.endsWith('/')) p.chop(1);
  path = p;
}

//////////////////////////////////////////////////////
/// ProjectGitLab

ProjectGitLab::ProjectGitLab(const QString &url, ChumPackage *package) : ProjectAbstract(package) {
  initSites();
  parseUrl(url, m_host, m_path);
  if (!s_sites.contains(m_host)) {
    qWarning() << "Shouldn't happen: ProjectGitLab initialized with incorrect service" << url;
    return;
  }

  m_token = s_sites.value(m_host, QString{});

  // url is not set as it can be different homepage that is retrieved from query
  m_package->setUrlIssues(QStringLiteral("https://%1/%2/-/issues").arg(m_host, m_path));

  // fetch information from GitLab
  fetchRepoInfo();
}

// static
void ProjectGitLab::initSites() {
  if (!s_sites.isEmpty()) return;
  for (const QString &sitetoken: QStringLiteral(GITLAB_TOKEN).split(QChar('|'))) {
      QStringList st = sitetoken.split(QChar(':'));
      if (st.size() != 2) {
          qWarning() << "Error parsing provided GitLab site-token pairs";
          return;
      }
      s_sites[st[0]] = st[1];
      qDebug() << "GitLab support added for" << st[0];
  }
}

// static
bool ProjectGitLab::isProject(const QString &url) {
  initSites();
  QString h, p;
  parseUrl(url, h, p);
  return s_sites.contains(h);
}

QNetworkReply* ProjectGitLab::sendQuery(const QString &query) {
  QString reqAuth = QStringLiteral("Bearer %1").arg(m_token);
  QString reqUrl = QStringLiteral("https://%1/api/graphql").arg(m_host);
  QNetworkRequest request;
  request.setUrl(reqUrl);
  request.setRawHeader("Content-Type", "application/json");
  request.setRawHeader("Authorization", reqAuth.toLocal8Bit());
  return nMng->post(request, query.toLocal8Bit());
}

void ProjectGitLab::fetchRepoInfo() {
  QString query = QStringLiteral(R"GR4PHQL(
{
"query": "
query {
  project(fullPath: "%1") {
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
)GR4PHQL").arg(m_path);
  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, this, [this, reply](){
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

    vi = r.value("forksCount").toInt(-1);
    if (vi>=0) m_package->setForksCount(vi);

    vi = r.value("openIssuesCount").toInt(-1);
    if (vi>=0) m_package->setIssuesCount(vi);

    vi = r.value("releases").toObject().value("count").toInt(-1);
    if (vi>=0) m_package->setReleasesCount(vi);

    reply->deleteLater();
  });
}


void ProjectGitLab::issue(const QString &issue_id, LoadableObject *value) {
  if (value->ready() && value->valueId()==issue_id)
    return; // value already corresponds to that release
  value->reset(issue_id);

  QString query = QStringLiteral(R"GR4PHQL(
{
"query": "
query {
  project(fullPath: "%1") {
    issue(iid: "%2") {
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
)GR4PHQL").arg(m_path, issue_id);

  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, this, [this, issue_id, reply, value](){
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

  QString query = QStringLiteral(R"GR4PHQL(
{
"query": "
query {
  project(fullPath: "%1") {
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
)GR4PHQL").arg(m_path);
  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, this, [this, issues_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch issues for" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject().
          value("issues").toObject().value("nodes").toArray().toVariantList();

    QVariantList rlist;
    for (const auto &e: r) {
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

  QString query = QStringLiteral(R"GR4PHQL(
{
"query": "
query {
  project(fullPath: "%1") {
    release(tagName: "%2") {
      name
      createdAt
      descriptionHtml
    }
  }
}"
}
)GR4PHQL").arg(m_path, release_id);

  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, this, [this, release_id, reply, value](){
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

  QString query = QStringLiteral(R"GR4PHQL(
{
"query": "
query {
  project(fullPath: "%1") {
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
)GR4PHQL").arg(m_path);
  query = query.replace('\n', ' ');

  QNetworkReply *reply = sendQuery(query);
  connect(reply, &QNetworkReply::finished, this, [this, releases_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "GitLab: Failed to fetch releases for" << this->m_path;
      qWarning() << "GitLab: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).object().
          value("data").toObject().value("project").toObject().
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
