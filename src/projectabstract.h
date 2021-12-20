#ifndef PROJECTABSTRACT_H
#define PROJECTABSTRACT_H

#include <QObject>

#include "loadableobject.h"

class ChumPackage;

class ProjectAbstract : public QObject
{
  Q_OBJECT
public:
  explicit ProjectAbstract(ChumPackage *package);

  virtual void release(const QString &id, LoadableObject *value) = 0;
  virtual void releases(LoadableObject *value) = 0;

signals:

protected:
  ChumPackage *m_package;
};

#endif // PROJECTABSTRACT_H
