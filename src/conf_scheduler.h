#ifndef CONF_SCHEDULER_H
#define CONF_SCHEDULER_H

#include <QObject>

namespace cfs
{

class conf_scheduler : public QObject
{
    Q_OBJECT
public:
    explicit conf_scheduler(QObject *parent = nullptr);

signals:

public slots:

};

} //namespace cfs

#endif // CONF_SCHEDULER_H
