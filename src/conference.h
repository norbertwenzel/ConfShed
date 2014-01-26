#ifndef CFS_CONFERENCE_H
#define CFS_CONFERENCE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QSharedPointer>
#include <QDir>
#include <QList>
#include <QQmlListProperty>

#include "conference_data.h"
#include "event.h"

namespace cfs
{

class conf_scheduler;

class conference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int conf_id READ conf_id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString subtitle READ subtitle CONSTANT)
    Q_PROPERTY(QString venue READ venue CONSTANT)
    Q_PROPERTY(QString city READ city CONSTANT)
    Q_PROPERTY(QString code READ code CONSTANT)
    Q_PROPERTY(QUrl remote_file READ remote_file CONSTANT)

    Q_PROPERTY(QQmlListProperty<cfs::event> events READ events CONSTANT)

public:
    explicit conference(QObject *parent = nullptr);
    explicit conference(const detail::conference_data &cd,
                        conf_scheduler *parent);
#ifndef NDEBUG
    ~conference();
#endif

    int conf_id() const { return id_; }
    QString title() const { return title_; }
    QString subtitle() const { return subtitle_; }
    QString venue() const { return venue_; }
    QString city() const { return city_; }
    QString code() const { return code_; }
    QUrl remote_file() const { return remote_file_; }
    QQmlListProperty<cfs::event> events();

    static QString compute_code(const QUrl &remote_data_url);

    void update_data(const detail::conference_data &cd);

signals:
    void eventsUpdated() const;

public slots:
    void update();

private:
    void create_events(const QList<detail::conference_data::event_data> &ed);

private:
    static const int INVALID_CONFERENCE_ID = 0;

    int id_;
    QString title_;
    QString subtitle_;
    QString venue_;
    QString city_;
    QString code_;
    QUrl remote_file_;

    QList<cfs::event*> events_;
};

} //namespace cfs

#endif //CFS_CONFERENCE_H
