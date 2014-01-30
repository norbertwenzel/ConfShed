#ifndef CFS_CONFERENCE_H
#define CFS_CONFERENCE_H

#include <memory>

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDateTime>
#include <QDir>
#include <QList>
#include <QQmlListProperty>

#include "conference_data.h"
#include "event.h"
#include "event_list_model.h"

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
    Q_PROPERTY(QDateTime starttime READ starttime CONSTANT)
    Q_PROPERTY(QDateTime endtime READ endtime CONSTANT)
    Q_PROPERTY(cfs::event_list_model* events READ events NOTIFY eventsChanged)

public:
    explicit conference(QObject *parent = nullptr);
    conference(const detail::conference_data &cd,
               conf_scheduler *parent);
    ~conference();

    int conf_id() const { return id_; }
    QString title() const { return title_; }
    QString subtitle() const { return subtitle_; }
    QString venue() const { return venue_; }
    QString city() const { return city_; }
    QString code() const { return code_; }
    QUrl remote_file() const { return remote_file_; }
    QDateTime starttime() const { return starttime_; }
    QDateTime endtime() const { return endtime_; }
    cfs::event_list_model* events() const;

    static QString compute_code(const QUrl &remote_data_url);

    void update_data(const detail::conference_data &cd);
    void star_event(const cfs::event &e);
    void unstar_event(const cfs::event &e);

signals:
    void eventsChanged() const;

public slots:
    void update(bool update_remote_data, bool update_full_event);
    void unsubscribe();

    //TODO use http://qt-project.org/doc/qt-5.0/qtcore/qsortfilterproxymodel.html instead
    void sort_events();
    void filter_events();

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
    QDateTime starttime_;
    QDateTime endtime_;

    QList<cfs::event*> events_;

private:
    struct cache;
    std::unique_ptr<cache> cache_;
};

} //namespace cfs

#endif //CFS_CONFERENCE_H
