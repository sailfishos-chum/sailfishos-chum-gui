#include "projectabstract.h"
#include "chumpackage.h"

ProjectAbstract::ProjectAbstract(ChumPackage *package) :
  QObject(package),
  m_package(package)
{

}
