#ifndef PROJECTABSTRACT_H
#define PROJECTABSTRACT_H

#include <QObject>

class ChumPackage;

class ProjectAbstract : public QObject
{
  Q_OBJECT
public:
  explicit ProjectAbstract(ChumPackage *package);

signals:

protected:
  ChumPackage *m_package;
};

#endif // PROJECTABSTRACT_H
