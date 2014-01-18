#include "conf_scheduler.h"

#include <QDebug>

using cfs::conf_scheduler;

conf_scheduler::conf_scheduler(QObject *parent) :
    QObject(parent)
{
}

void conf_scheduler::addConference(const QUrl &conf_data_url)
{
    qDebug() << conf_data_url;
    emit conferenceAdded(conf_data_url);
}
