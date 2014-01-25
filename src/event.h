#ifndef CFS_EVENT_H
#define CFS_EVENT_H

#include <QObject>
#include <QString>
#include <QQmlListProperty>
#include <QDateTime>
#include <QScopedPointer>

#include "conference_data.h"

namespace cfs
{

class conference;

class event : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int event_id READ event_id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString subtitle READ subtitle CONSTANT)
    Q_PROPERTY(QString track READ track CONSTANT)
    Q_PROPERTY(QString abstract READ abstract CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString room READ room CONSTANT)
    Q_PROPERTY(QList<QString> persons READ persons CONSTANT)
    Q_PROPERTY(QDateTime starttime READ starttime CONSTANT)
    Q_PROPERTY(QDateTime endtime READ endtime CONSTANT)

public:
    explicit event(QObject *parent = nullptr);
    explicit event(const detail::conference_data::event_data &data,
                   cfs::conference *parent);

    int event_id() const { return id_; }
    QString title() const { return title_; }
    QString subtitle() const { return subtitle_; }
    QString track() const { return track_; }
    QString abstract() const { return abstract_; }
    QString description() const { return description_; }
    QString room() const { return room_; }
    QList<QString> persons() const { return persons_; }
    QDateTime starttime() const { return start_; }
    QDateTime endtime() const { return end_; }

signals:

public slots:

private:
    int id_;
    QString title_;
    QString subtitle_;
    QString track_;
    QString abstract_;
    QString description_;
    QString room_;
    QList<QString> persons_;
    QDateTime start_;
    QDateTime end_;
};

} //namespace cfs

#endif //CFS_EVENT_H
