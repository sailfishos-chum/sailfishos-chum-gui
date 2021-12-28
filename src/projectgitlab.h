#ifndef PROJECTGITLAB_H
#define PROJECTGITLAB_H

#include <QObject>
#include <QNetworkReply>
#include <QString>

#include "projectabstract.h"

class ProjectGitLab : public ProjectAbstract
{
  Q_OBJECT
public:
  explicit ProjectGitLab(const QString &url, ChumPackage *package);

  static bool isProject(const QString &url);

  virtual void issue(const QString &id, LoadableObject *value) override;
  virtual void issues(LoadableObject *value) override;
  virtual void release(const QString &id, LoadableObject *value) override;
  virtual void releases(LoadableObject *value) override;

signals:

private:
  QNetworkReply* sendQuery(const QString &query);
  void fetchRepoInfo();

  static void initSites();

private:
  QString m_host;
  QString m_path;
  QString m_token;

  static QMap<QString, QString> s_sites;
};

#endif // PROJECTGITLAB_H
