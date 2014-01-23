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

class conf_scheduler : public QObject
{
    Q_OBJECT
public:
    explicit conf_scheduler(QObject *parent = nullptr);
    ~conf_scheduler();

    Q_INVOKABLE int get_num_conferences() const;
    Q_INVOKABLE cfs::conference_list_model* get_all_conferences() const;

signals:
    void conferenceListChanged(cfs::conference_list_model *confs) const;
    void conferenceAdded(conference *conf) const;
    void error(const QString &message) const;

public slots:
    void addConference(const QUrl &conf_data_url);
    void updateAllConferences();

private:
    static QDir get_existing_data_dir();
    static QUrl get_data_file_location(QString code, const QString &ext);

    void download_conf_data(const QUrl &remote_conf_data_url, const QUrl &local_data_file) const;
    std::unique_ptr<detail::conference_data> parse_conference_header(const QUrl &local_data_file) const;
    std::unique_ptr<detail::conference_data> parse_conference_complete(const QUrl &local_data_file) const;

private:
    static const QString STORAGE_IDENTIFIER;
    std::unique_ptr<detail::storage> storage_;
};

} //namespace cfs

#endif //CFS_CONF_SCHEDULER_H
