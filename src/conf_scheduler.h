#ifndef CFS_CONF_SCHEDULER_H
#define CFS_CONF_SCHEDULER_H

#include <memory>

#include <QObject>
#include <QUrl>
#include <QDir>

#include "conference.h"

namespace cfs
{

namespace detail
{
class storage;
} //namespace detail

class conf_scheduler : public QObject
{
    Q_OBJECT
public:
    explicit conf_scheduler(QObject *parent = nullptr);
    ~conf_scheduler();

    Q_INVOKABLE int get_num_conferences() const;

signals:
    void conferenceAdded(conference *conf) const;
    void error(const QString &message) const;

public slots:
    void addConference(const QUrl &conf_data_url);

private:
    static QDir get_existing_data_dir();

private:
    static const QString STORAGE_IDENTIFIER;
    std::unique_ptr<detail::storage> storage_;
};

} //namespace cfs

#endif //CFS_CONF_SCHEDULER_H
