#ifndef CFS_CONF_SCHEDULER_H
#define CFS_CONF_SCHEDULER_H

#include <QObject>
#include <QUrl>

namespace cfs
{

class conf_scheduler : public QObject
{
    Q_OBJECT
public:
    explicit conf_scheduler(QObject *parent = nullptr);
#ifndef NDEBUG
    ~conf_scheduler();
#endif

signals:
    void conferenceAdded(const QUrl &conf_data_url) const;
    void error(const QString &message) const;

public slots:
    void addConference(const QUrl &conf_data_url);
};

} //namespace cfs

#endif //CFS_CONF_SCHEDULER_H
