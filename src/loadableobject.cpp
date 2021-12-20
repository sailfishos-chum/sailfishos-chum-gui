#include "loadableobject.h"

#include <QDebug>

LoadableObject::LoadableObject(QObject *parent) :
  QObject(parent)
{
}

void LoadableObject::reset(const QString &value_id) {
  m_ready = false;
  m_value_id = value_id;
  m_value.clear();
  emit readyChanged();
  emit valueChanged();
}

void LoadableObject::setEmpty() {
  m_ready = true;
  m_value.clear();
  emit readyChanged();
  emit valueChanged();
}

void LoadableObject::setValue(const QString &value_id, const QVariantMap &value) {
  if (m_value_id != value_id) return;
  m_ready = true;
  m_value = value;
  emit readyChanged();
  emit valueChanged();
}
