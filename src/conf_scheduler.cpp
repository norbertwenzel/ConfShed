#include "conf_scheduler.h"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <sstream>

#include <QtGlobal>
#include <QDebug>
#include <QStandardPaths>

#include "conference.h"
#include "storage.h"
#include "sync_downloader.h"
#include "conference_data.h"
#include "pentabarf_parser.h"

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

cfs::conference_list_model* conf_scheduler::get_all_conferences() const
{
    QList<cfs::conference*> all_confs;
    try
    {
        assert(storage_);
        const auto &all_data = storage_->get_conferences();
        qDebug() << "Trying to load" << all_data.size() << "conferences.";

        all_confs.reserve(all_data.size());
        std::for_each(std::begin(all_data), std::end(all_data),
        [&](decltype(*std::begin(all_data)) &d)
        {
            all_confs.push_back(new conference(d, const_cast<conf_scheduler*>(this)));
        });

        assert(all_confs.size() == get_num_conferences());
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        all_confs.clear();
    }

#ifndef NDEBUG
    //make sure we have only cfs::conference* in that list
    std::for_each(std::begin(all_confs), std::end(all_confs),
    [](decltype(*std::begin(all_confs)) &c)
    {
        assert(qobject_cast<cfs::conference*>(c));
    });
#endif

    return new cfs::conference_list_model(all_confs, const_cast<conf_scheduler*>(this));
}

void conf_scheduler::addConference(const QUrl &remote_conf_data_url)
{
    qDebug() << remote_conf_data_url;
    try
    {
        //compute a proper conference code
        const auto &code = conference::compute_code(remote_conf_data_url);

        //TODO check if conference has already been added

        //compute a valid data file location
        const auto &local_data_file = get_data_file_location(code, ".xml");
        qDebug() << "data_file: " << local_data_file << ", code: " << code;

        //download the file
        sync_downloader dl;
        if(dl.download_to(remote_conf_data_url, local_data_file))
        {
            const auto parser = std::unique_ptr<detail::conference_parser>(new detail::pentabarf_parser());

            QFile file(local_data_file.path());
            if(file.exists())
            {
                //TODO parse the conference data *only*
                auto data = parser->parse(file);

                //add the conference to the database
                data.code = code;
                data.remote_data = remote_conf_data_url;
                data.id = storage_->add_or_update_conference(data);

                //TODO maybe this should be async, so we return as soon as there's data in the DB
                //TODO parse the conference event data

                //TODO use boost scope_guard to rollback in case of errors

                auto conf = new conference(data, this);
                emit conferenceAdded(conf);
                return;
            }
            else
            {
                throw std::runtime_error("Failed to read conference data.");
            }
        }
        else
        {
            throw std::runtime_error((QString("Failed to download ") + remote_conf_data_url.toString()).toLocal8Bit().data());
        }
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        return;
    }

    emit error("Unknown error adding conference.");
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

QUrl conf_scheduler::get_data_file_location(QString code, const QString &extension)
{
    return QUrl::fromLocalFile(get_existing_data_dir().filePath(code.append(extension))).path();
}
