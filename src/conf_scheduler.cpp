#include "conf_scheduler.h"

#include <exception>

#include <QDebug>

#include "conference.h"

using cfs::conf_scheduler;

namespace cfs
{
class storage
{
};
}

conf_scheduler::conf_scheduler(QObject *parent) :
    QObject(parent)
{
}

conf_scheduler::~conf_scheduler()
{
    //empty dtor because of forward declared storage in unique_ptr
    qDebug();
}

int conf_scheduler::get_num_conferences() const
{
    //TODO: add the database as a private member in here and actually read the value
    return 1;
}

void conf_scheduler::addConference(const QUrl &conf_data_url)
{
    qDebug() << conf_data_url;
    try
    {
        auto conf = conference::from_file(conf_data_url, this);
        emit conferenceAdded(conf);
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
    }
}
