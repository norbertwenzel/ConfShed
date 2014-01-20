#include "conf_scheduler.h"

#include <cassert>
#include <exception>
#include <sstream>

#include <QtGlobal>
#include <QDebug>
#include <QStandardPaths>

#include "conference.h"
#include "storage.h"

using cfs::conf_scheduler;

const QString conf_scheduler::STORAGE_IDENTIFIER("data.db");

conf_scheduler::conf_scheduler(QObject *parent) :
    QObject(parent),
    storage_(new detail::storage(get_existing_data_dir().filePath(STORAGE_IDENTIFIER)))
{
    assert(storage_);
}

conf_scheduler::~conf_scheduler()
{
    //empty dtor because of forward declared storage in unique_ptr
    qDebug();
}

int conf_scheduler::get_num_conferences() const
{
    try
    {
        assert(storage_);
        return storage_->get_num_conferences();
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        return -1;
    }
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

QDir conf_scheduler::get_existing_data_dir()
{
    const auto &dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if(!dir.exists() && !dir.mkpath(dir.path()))
    {
        //TODO: use qt string builder instead
        std::ostringstream msg;
        msg << "Data directory '" << dir.path().toLocal8Bit().data() << "' does not exist, " <<
               "and can not be created.";
        //TODO: maybe calling emit from a ctor is not the best idea
        //emit error(QString::fromLocal8Bit(msg.str().c_str()));
        qFatal(msg.str().c_str());
        exit(1);
    }

    assert(dir.exists());
    return dir;
}
