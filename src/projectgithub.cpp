#include "projectgithub.h"
#include "chumpackage.h"
#include "main.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

static QString reqAuth{QStringLiteral("bearer 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")};
static QString reqUrl{QStringLiteral("https://api.github.com/graphql")};

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

ProjectGitHub::ProjectGitHub(const QString &url, ChumPackage *package) : ProjectAbstract(package) {
  bool ok = parseUrl(url, m_org, m_repo);
  if (!ok) {
    qWarning() << "Shouldn't happen: ProjectGitHub initialized with incorrect service" << url;
    return;
  }

  qDebug() << "Github" << m_org << m_repo;

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

  QNetworkRequest request;
  request.setUrl(reqUrl);
  request.setRawHeader("Authorization", reqAuth.toLocal8Bit());
  QNetworkReply *reply = nMng->post(request, query.toLocal8Bit());
  connect(reply, &QNetworkReply::finished, [this, reply](){
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Failed to fetch repository data for" << this->m_org << this->m_repo;
      qWarning() << "Error: " << reply->errorString();
    }

    QJsonObject r{QJsonDocument::fromJson(reply->readAll()).object().
          value("data").toObject().value("repository").toObject()};

    QString v;
    int vi;

    v = r.value("owner").toObject().value("login").toString();
    if (!v.isEmpty()) m_package->setDeveloperLogin(v);

    v = r.value("owner").toObject().value("name").toString();
    if (!v.isEmpty()) m_package->setDeveloperName(v);

    vi = r.value("stargazerCount").toInt(-1);
    if (vi>=0) m_package->setStarsCount(vi);

    v = r.value("homepageUrl").toString();
    if (!v.isEmpty()) m_package->setUrl(v);
    else m_package->setUrl(QStringLiteral("https://github.com/%1/%2").arg(m_org, m_repo));

    vi = r.value("forks").toObject().value("totalCount").toInt();
    if (vi>=0) m_package->setForksCount(vi);

    vi = r.value("issues").toObject().value("totalCount").toInt();
    if (vi>=0) m_package->setIssuesCount(vi);

    vi = r.value("releases").toObject().value("totalCount").toInt();
    if (vi>=0) m_package->setReleasesCount(vi);

    vi = r.value("discussions").toObject().value("totalCount").toInt();
    if (vi>=0)
      m_package->setUrlForum(QStringLiteral("https://github.com/%1/%2/discussions").arg(m_org, m_repo));

    reply->deleteLater();
  });
}
