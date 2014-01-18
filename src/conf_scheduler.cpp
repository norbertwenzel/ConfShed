#include "conf_scheduler.h"

#include <exception>

#include <QDebug>

#include "conference.h"

using cfs::conf_scheduler;

conf_scheduler::conf_scheduler(QObject *parent) :
    QObject(parent)
{
}

#ifndef NDEBUG
conf_scheduler::~conf_scheduler()
{
    qDebug();
}
#endif

void conf_scheduler::addConference(const QUrl &conf_data_url)
{
    qDebug() << conf_data_url;
    try
    {
        auto conf = conference::from_file(conf_data_url, this);
        emit conferenceAdded(conf_data_url);
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
    }
}
