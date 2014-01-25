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
        const auto parent_ptr = const_cast<conf_scheduler*>(this);
        std::transform(std::begin(all_data), std::end(all_data),
                       std::back_inserter(all_confs),
        [&](decltype(*std::begin(all_data)) &d)
        {
            return new conference(d, parent_ptr);
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

        download_conf_data(remote_conf_data_url, local_data_file);
        //TODO parse the conference data *only*
        const auto data = parse_conference_header(local_data_file);

        //add the conference to the database
        data->code = code;
        data->remote_data = remote_conf_data_url;
        data->id = storage_->add_or_update_conference(*data);

        //TODO maybe this should be async, so we return as soon as there's data in the DB
        //TODO parse the conference event data

        //TODO use boost scope_guard to rollback in case of errors

        auto conf = new conference(*data, this);
        emit conferenceAdded(conf);
        return;
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        return;
    }

    emit error("Unknown error adding conference.");
}

void conf_scheduler::removeConference(cfs::conference *conf)
{
    if(!conf)
    {
        qDebug() << "(nullptr)";
        return;
    }

    qDebug() << conf->conf_id() << conf->title();

    try
    {
        //delete data file
        QFile file(get_data_file_location(conf->code(), ".xml").path());
        assert(file.exists());
        if(file.exists())
        {
            if(!file.remove()) throw std::runtime_error("Could not delete conference data.");
        }

        //delete item from db
        assert(storage_);
        storage_->delete_conference(conf->conf_id());

        //TODO: clear the conf object to free as much memory as possible
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        return;
    }

    emit conferenceListChanged(get_all_conferences());
}

void conf_scheduler::updateConference(cfs::conference *conf)
{
    if(!conf)
    {
        qDebug() << "(nullptr)";
        return;
    }

    qDebug() << conf->conf_id() << conf->title();

    try
    {
        do_update_conference(conf, true, true);
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        return;
    }
}

void conf_scheduler::updateAllConferences()
{
    qDebug() << __FUNCTION__;

    QList<cfs::conference*> all_confs;
    try
    {
        assert(storage_);
        const auto &all_data = storage_->get_conferences();
        qDebug() << "Trying to update" << all_data.size() << "conferences.";

        all_confs.reserve(all_data.size());

        //TODO: this should maybe run concurrenty
        std::for_each(std::begin(all_data), std::end(all_data),
        [&](decltype(*std::begin(all_data)) &d)
        {
            //compute a valid data file location
            const auto &local_data_file = get_data_file_location(d.code, ".xml");

            download_conf_data(d.remote_data, local_data_file);
            //TODO parse the conference data *only*
            const auto &data = parse_conference_header(local_data_file);
            assert(data);

            //update the conference in the database
            data->id = d.id;
            data->code = d.code;
            data->remote_data = d.remote_data;
            qDebug() << "Updating conference" << d.id << d.title << "check:" << data->id << data->title;
            data->id = storage_->add_or_update_conference(*data);

            all_confs.push_back(new conference(*data, const_cast<conf_scheduler*>(this)));
        });

        assert(all_confs.size() == get_num_conferences());
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        all_confs.clear();
        return;
    }

    emit conferenceListChanged(new cfs::conference_list_model(all_confs, const_cast<conf_scheduler*>(this)));
}

void conf_scheduler::download_conf_data(const QUrl &remote_conf_data_url, const QUrl &local_data_file) const
{
    sync_downloader dl;
    if(dl.download_to(remote_conf_data_url, local_data_file))
    {
    }
    else
    {
        throw std::runtime_error((QString("Failed to download ") + remote_conf_data_url.toString()).toLocal8Bit().data());
    }
}

std::unique_ptr<cfs::detail::conference_data> conf_scheduler::parse_conference_header(const QUrl &local_data_file) const
{
    const auto parser = std::unique_ptr<detail::conference_parser>(new detail::pentabarf_parser());

    QFile file(local_data_file.path());
    if(file.exists())
    {
        return parser->parse(file, detail::conference_parser::PARSE_WITHOUT_EVENTS);
    }
    else
    {
        throw std::runtime_error("Failed to read conference data.");
    }
}

std::unique_ptr<cfs::detail::conference_data> conf_scheduler::parse_conference_complete(const QUrl &local_data_file) const
{
    const auto parser = std::unique_ptr<detail::conference_parser>(new detail::pentabarf_parser());

    QFile file(local_data_file.path());
    if(file.exists())
    {
        return parser->parse(file);
    }
    else
    {
        throw std::runtime_error("Failed to read conference data.");
    }
}

void conf_scheduler::do_update_conference(cfs::conference *conf,
                                          bool update_remote_file,
                                          bool update_all_events)
{
    assert(storage_);
    const auto &db_data = storage_->get_conference(conf->conf_id());

    //compute a valid data file location
    const auto &local_data_file = get_data_file_location(db_data.code, ".xml");

    //update the remote file if necessary
    if(update_remote_file)
    {
        download_conf_data(db_data.remote_data, local_data_file);
    }

    //parse the necessary conference data
    decltype(parse_conference_complete(local_data_file)) data;
    if(update_all_events)
    {
        data = parse_conference_complete(local_data_file);
    }
    else
    {
        data = parse_conference_header(local_data_file);
    }
    assert(data);

    //update the conference in the database
    data->id = db_data.id;
    data->code = db_data.code;
    data->remote_data = db_data.remote_data;
    data->id = storage_->add_or_update_conference(*data);

    //update the conference
    conf->update_data(*data);
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
