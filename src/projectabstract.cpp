#include "projectabstract.h"
#include "chumpackage.h"

#include <QLocale>

ProjectAbstract::ProjectAbstract(ChumPackage *package) :
    QObject(package),
    m_package(package)
{
}

QString ProjectAbstract::parseDate(QString txt, bool short_format) {
    QDateTime dt = QDateTime::fromString(txt, Qt::ISODate);
    return QLocale::system().toString(dt.toLocalTime().date(),
                                      short_format ? QLocale::ShortFormat :
                                                     QLocale::LongFormat);
}
