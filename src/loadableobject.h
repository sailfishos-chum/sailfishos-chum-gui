#ifndef LOADABLEOBJECT_H
#define LOADABLEOBJECT_H

#include <QObject>
#include <QVariantMap>

class LoadableObject : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
  Q_PROPERTY(QVariantMap value READ value NOTIFY valueChanged)

public:
  explicit LoadableObject(QObject *parent = nullptr);

  bool ready() const { return m_ready; }
  QVariantMap value() const { return m_value; }

  void reset(const QString &value_id);
  void setEmpty();
  void setValue(const QString &value_id, const QVariantMap &value);

signals:
  void readyChanged();
  void valueChanged();

private:
  bool m_ready{false};
  QVariantMap m_value;
  QString m_value_id;
};

#endif // LOADABLEOBJECT_H
