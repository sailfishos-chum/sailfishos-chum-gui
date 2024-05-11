#include "projectforgejo.h"
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

QMap<QString, QString> ProjectForgejo::s_sites;

//////////////////////////////////////////////////////
/// helper functions

static QString getName(const QVariant &v) {
  QVariantMap m = v.toMap();
  QString login = m[QLatin1String("username")].toString();
  QString name = m[QLatin1String("full_name")].toString();
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
/// ProjectForgejo

ProjectForgejo::ProjectForgejo(const QString &url, ChumPackage *package) : ProjectAbstract(package) {
  initSites();
  parseUrl(url, m_host, m_path);
  if (!s_sites.contains(m_host)) {
    qWarning() << "Shouldn't happen: ProjectForgejo initialized with incorrect service" << url;
    return;
  }

  m_token = s_sites.value(m_host, QString{});

  // url is not set as it can be different homepage that is retrieved from query
  m_package->setUrlIssues(QStringLiteral("https://%1/%2/issues").arg(m_host, m_path));

  // fetch information from Forgejo
  fetchRepoInfo();
}

// static
void ProjectForgejo::initSites() {
  if (!s_sites.isEmpty()) return;
  for (const QString &sitetoken: QStringLiteral(FORGEJO_TOKEN).split(QChar('|'))) {
      QStringList st = sitetoken.split(QChar(':'));
      if (st.size() != 2) {
          qWarning() << "Error parsing provided Forgejo site-token pairs";
          return;
      }
      s_sites[st[0]] = st[1];
      qDebug() << "Forgejo support added for" << st[0];
  }
}

// static
bool ProjectForgejo::isProject(const QString &url) {
  initSites();
  QString h, p;
  parseUrl(url, h, p);
  return s_sites.contains(h);
}

QNetworkReply* ProjectForgejo::sendQuery(const QString &query) {
  QString reqAuth = QStringLiteral("token %1").arg(m_token);
  QString reqUrl = QStringLiteral("https://%1/api/v1%2").arg(m_host).arg(query);
  QNetworkRequest request;
  request.setUrl(reqUrl);
  request.setRawHeader("Content-Type", "application/json");
  request.setRawHeader("Authorization", reqAuth.toLocal8Bit());
  return nMng->get(request);
}

void ProjectForgejo::fetchRepoInfo() {
  QNetworkReply *reply = sendQuery(QStringLiteral("/repos/%1").arg(m_path));
  connect(reply, &QNetworkReply::finished, this, [this, reply](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Forgejo: Failed to fetch repository data for Forgejo" << this->m_path;
      qWarning() << "Forgejo: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QJsonObject r = QJsonDocument::fromJson(data).object();

    QString v;
    int vi;

    v = r.value("owner").toObject().value("login").toString();
    if (!v.isEmpty()) m_package->setDeveloperLogin(v);

    v = r.value("owner").toObject().value("full_name").toString();
    if (!v.isEmpty()) m_package->setDeveloperName(v);

    v = r.value("website").toString();
    if (!v.isEmpty()) m_package->setUrl(v);

    vi = r.value("stars_count").toInt(-1);
    if (vi>=0) m_package->setStarsCount(vi);

    vi = r.value("forks_count").toInt(-1);
    if (vi>=0) m_package->setForksCount(vi);

    vi = r.value("open_issues_count").toInt(-1);
    if (vi>=0) m_package->setIssuesCount(vi);

    vi = r.value("release_counter").toInt(-1);
    if (vi>=0) m_package->setReleasesCount(vi);

    reply->deleteLater();
  });
}

void ProjectForgejo::comments(const QString &issue_id, const QVariantMap &comment, LoadableObject *value) {
  QNetworkReply *reply = sendQuery( QStringLiteral("/repos/%1/issues/%2/comments").arg(m_path).arg(issue_id));
  connect(reply, &QNetworkReply::finished, this, [this, issue_id, comment, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Forgejo: Failed to fetch issue for" << this->m_path;
      qWarning() << "Forgejo: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).array().toVariantList();

    QVariantList rlist;
    rlist.append(comment); // add issue comment as first comment
    for (const auto &e: r) {
      QVariantMap element = e.toMap();
      QVariantMap m;
      m["author"] = getName(element.value("user"));
      m["created"] = parseDate(element.value("created_at").toString(), true);
      m["updated"] = parseDate(element.value("updated_at").toString(), true);
      m["body"] = element.value("body").toString();
      rlist.append(m);
    }

    QVariantMap result = value->value();
    result["discussion"] = rlist;

    value->setValue(issue_id, result);
    reply->deleteLater();
  });
}

void ProjectForgejo::issue(const QString &issue_id, LoadableObject *value) {
  if (value->ready() && value->valueId()==issue_id)
    return; // value already corresponds to that issue
  value->reset(issue_id);

  QNetworkReply *reply = sendQuery( QStringLiteral("/repos/%1/issues/%2").arg(m_path).arg(issue_id));
  connect(reply, &QNetworkReply::finished, this, [this, issue_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Forgejo: Failed to fetch issue for" << this->m_path;
      qWarning() << "Forgejo: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantMap r = QJsonDocument::fromJson(data).object().toVariantMap();

    QVariantMap result;
    result["id"] = r.value("number").toInt();
    result["number"] = r.value("number");
    result["title"] = r.value("title");
    result["commentsCount"] = r.value("comments").toInt();

    value->setValue(issue_id, result);

    // load comments separately: save "first comment"
    QVariantMap commentZero;
    commentZero["author"] = getName(r.value("user"));
    commentZero["created"] = parseDate(r.value("created_at").toString(), true);
    commentZero["updated"] = parseDate(r.value("updated_at").toString(), true);
    // FIXME: this is stored as markdown, not HTML
    //        maybe use /miscellaneous/renderMarkdown to get HTML
    commentZero["body"] = r.value("body").toString();

    // load comments
    comments(result["id"].toString(), commentZero, value);

    reply->deleteLater();
  });
}

void ProjectForgejo::issues(LoadableObject *value) {
  const QString issues_id{QStringLiteral("issues")};
  value->reset(issues_id);

  QNetworkReply *reply = sendQuery( QStringLiteral("/repos/%1/%2?state=open&type=issue").arg(m_path).arg(issues_id));

  connect(reply, &QNetworkReply::finished, this, [this, issues_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Forgejo: Failed to fetch issues for" << this->m_path;
      qWarning() << "Forgejo: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).array().toVariantList();

    QVariantList rlist;
    for (const auto &e: r) {
      QVariantMap element = e.toMap();
      QVariantMap m;
      m["id"] = element.value("number");
      m["author"] = getName(element.value("user"));
      m["commentsCount"] = element.value("comments");
      m["number"] = element.value("number");
      m["title"] = element.value("title");
      m["created"] = parseDate(element.value("created_at").toString(), true);
      m["updated"] = parseDate(element.value("updated_at").toString(), true);
      rlist.append(m);
    }

    QVariantMap result;
    result["issues"] = rlist;
    value->setValue(issues_id, result);

    reply->deleteLater();
  });
}


void ProjectForgejo::release(const QString &release_id, LoadableObject *value) {
  if (value->ready() && value->valueId()==release_id)
    return; // value already corresponds to that release
  value->reset(release_id);

  QNetworkReply *reply = sendQuery( QStringLiteral("/repos/%1/releases/%2").arg(m_path).arg(release_id));

  connect(reply, &QNetworkReply::finished, this, [this, release_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Forgejo: Failed to fetch release for" << this->m_path;
      qWarning() << "Forgejo: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantMap r = QJsonDocument::fromJson(data).object().toVariantMap();

    QVariantMap result;
    result["name"] = r.value("name");
    result["description"] = r.value("body");
    result["datetime"] = parseDate(r.value("created_at").toString());

    value->setValue(release_id, result);
    reply->deleteLater();
  });
}


void ProjectForgejo::releases(LoadableObject *value) {
  const QString releases_id{QStringLiteral("releases")};
  value->reset(releases_id);

  QNetworkReply *reply = sendQuery( QStringLiteral("/repos/%1/%2?pre-release=true").arg(m_path).arg(releases_id));

  connect(reply, &QNetworkReply::finished, this, [this, releases_id, reply, value](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Forgejo: Failed to fetch releases for" << this->m_path;
      qWarning() << "Forgejo: Error: " << reply->errorString();
    }

    QByteArray data = reply->readAll();
    QVariantList r = QJsonDocument::fromJson(data).array().toVariantList();

    QVariantList rlist;
    for (const auto &e: r) {
      QVariantMap element = e.toMap();
      QVariantMap m;
      m["id"] = element.value("id");
      m["name"] = element.value("name");
      m["datetime"] = parseDate(element.value("created_at").toString());
      rlist.append(m);
    }

    QVariantMap result;
    result["releases"] = rlist;
    value->setValue(releases_id, result);

    reply->deleteLater();
  });
}
