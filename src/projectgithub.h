#ifndef PROJECTGITHUB_H
#define PROJECTGITHUB_H

#include <QObject>
#include <QString>

#include "projectabstract.h"

class ProjectGitHub : public ProjectAbstract
{
  Q_OBJECT
public:
  explicit ProjectGitHub(const QString &url, ChumPackage *package);

  static bool isProject(const QString &url);

signals:

private:
  void fetchRepoInfo();

private:
  QString m_org;
  QString m_repo;

};

#endif // PROJECTGITHUB_H
