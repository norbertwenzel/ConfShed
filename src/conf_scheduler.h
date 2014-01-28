#ifndef CFS_CONF_SCHEDULER_H
#define CFS_CONF_SCHEDULER_H

#include <memory>

#include <QObject>
#include <QUrl>
#include <QDir>

#include "conference.h"
#include "conference_list.h"

namespace cfs
{

namespace detail
{
class storage;
class conference_data;
} //namespace detail

class event;

class conf_scheduler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool DEBUG READ is_debug CONSTANT)

public:
    explicit conf_scheduler(QObject *parent = nullptr);
    ~conf_scheduler();

    Q_INVOKABLE int get_num_conferences() const;
    Q_INVOKABLE cfs::conference_list_model* get_all_conferences() const;

    void star_event(const cfs::conference &conf, const cfs::event &evnt);
    void unstar_event(const cfs::conference &conf, const cfs::event &evnt);

    bool is_debug() const
    {
#ifndef NDEBUG
        return true;
#else
        return false;
#endif
    }

signals:
    void conferenceListChanged(cfs::conference_list_model *confs) const;
    void conferenceAdded(cfs::conference *conf) const;
    void error(const QString &message) const;

public slots:
    void addConference(const QUrl &conf_data_url);
    void removeConference(cfs::conference *conf);
    void updateConference(cfs::conference *conf, bool update_remote_data, bool update_full_event);
    void updateAllConferences();

private:
    static QDir get_existing_data_dir();
    static QUrl get_data_file_location(QString code, const QString &ext);

    void download_conf_data(const QUrl &remote_conf_data_url, const QUrl &local_data_file) const;
    std::unique_ptr<detail::conference_data> parse_conference_header(const QUrl &local_data_file) const;
    std::unique_ptr<detail::conference_data> parse_conference_complete(const QUrl &local_data_file) const;

    void do_update_conference(cfs::conference *conf, bool update_remote_file, bool update_full_event);

private:
    static const QString STORAGE_IDENTIFIER;
    std::unique_ptr<detail::storage> storage_;
};

} //namespace cfs

#endif //CFS_CONF_SCHEDULER_H
