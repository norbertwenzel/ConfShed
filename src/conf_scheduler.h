#ifndef CONF_SCHEDULER_H
#define CONF_SCHEDULER_H

#include <QObject>
#include <QUrl>

namespace cfs
{

class conf_scheduler : public QObject
{
    Q_OBJECT
public:
    explicit conf_scheduler(QObject *parent = nullptr);

signals:
    void conferenceAdded(const QUrl &conf_data_url) const;

public slots:
    void addConference(const QUrl &conf_data_url);
};

} //namespace cfs

#endif // CONF_SCHEDULER_H
