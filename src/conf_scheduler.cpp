#include "conf_scheduler.h"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <vector>

#include <boost/numeric/conversion/cast.hpp>

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
    storage_(new detail::storage(get_existing_data_dir().filePath(STORAGE_IDENTIFIER))),
    conferences_(new conference_list_model(this))
{
    assert(storage_);
    assert(conferences_);
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
    assert(conferences_);

    //prefer cached list of conferences if available
    if(conferences_->rowCount() > 0)
    {
        return conferences_;
    }

    try
    {
        std::vector<cfs::conference*> all_confs;

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

        assert(all_confs.size() == boost::numeric_cast<decltype(all_confs.size())>(get_num_conferences()));

        //add all conferences to the list model
        std::for_each(std::begin(all_confs), std::end(all_confs),
        [&](cfs::conference *conf)
        {
            conferences_->add_conference(conf);
        });
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
    }

    return conferences_;
}

void conf_scheduler::star_event(const cfs::conference &conf, const cfs::event &evnt)
{
    try
    {
        assert(storage_);
        storage_->add_favorite(conf.conf_id(), evnt.event_id());
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        throw;
    }
}

void conf_scheduler::unstar_event(const cfs::conference &conf, const cfs::event &evnt)
{
    try
    {
        assert(storage_);
        storage_->remove_favorite(conf.conf_id(), evnt.event_id());
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        throw;
    }
}

void conf_scheduler::addConference(const QUrl &remote_conf_data_url)
{
    qDebug() << remote_conf_data_url;

    assert(conferences_);

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

        conferences_->add_conference(new conference(*data, this));
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
        assert(conf);

        //check if the given conference is actually ours
        if(conferences_ && conferences_->get(conf->conf_id()) == conf)
        {
            //delete data file
            QFile file(get_data_file_location(conf->code(), ".xml").path());
            if(file.exists())
            {
                if(!file.remove()) throw std::runtime_error("Could not delete conference data.");
            }
            assert(!file.exists());

            //delete item from db
            assert(storage_);
            storage_->delete_conference(conf->conf_id());

            //clear the actual conf object
            conf->deleteLater();
        }
        else
        {
            qDebug() << conf << conf->title() << ":" << conf->conf_id() <<
                        "is unhandled in here. Ignored.";
            static const bool tried_to_delete_unhandled_conf = false;
            assert(tried_to_delete_unhandled_conf);
            return;
        }
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        return;
    }

    //emit conferenceListChanged(get_all_conferences());
}

void conf_scheduler::updateConference(cfs::conference *conf, bool update_remote_data, bool update_full_event)
{
    if(!conf)
    {
        qDebug() << "(nullptr)";
        return;
    }

    qDebug() << conf->conf_id() << conf->title();

    try
    {
        do_update_conference(conf, update_remote_data, update_full_event);
        emit conf->eventsChanged(); //TODO: call signal inside conference and only when necessary
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

    QList<cfs::conference*> all_confs = findChildren<cfs::conference*>();
#ifndef NDEBUG
    std::for_each(std::begin(all_confs), std::end(all_confs),
    [](decltype(*std::begin(all_confs)) &conf)
    {
        assert(conf);
    });
#endif
    try
    {
        qDebug() << "Trying to update" << all_confs.size() << "conferences.";

        //TODO: this should maybe run concurrenty
        std::for_each(std::begin(all_confs), std::end(all_confs),
        [&](decltype(*std::begin(all_confs)) &conf)
        {
            do_update_conference(conf, true, false);
        });

        assert(all_confs.size() == get_num_conferences());
    }
    catch(const std::exception &e)
    {
        emit error(QString::fromLocal8Bit(e.what()));
        all_confs.clear();
        return;
    }

    //TODO: possibly update an already existing conference_list_model child instead of creating a new one
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
                                          bool update_full_event)
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
    if(update_full_event)
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

    //check the favorite list for the current conference and set the event status appropriately
    if(data->events.size() > 0)
    {
        auto favorites = storage_->get_favorites(data->id);
        std::sort(std::begin(favorites), std::end(favorites));
        assert(std::is_sorted(std::begin(favorites), std::end(favorites)));

        std::for_each(std::begin(data->events), std::end(data->events),
        [&](decltype(*std::begin(data->events)) &evnt)
        {
            evnt.favorite = std::binary_search(std::begin(favorites), std::end(favorites), evnt.id);
#ifndef NDEBUG
            if(evnt.favorite)
            {
                qDebug() << data->id << ":" << data->title << "contains favorite" <<
                            evnt.id << ":" << evnt.title;
            }
#endif
        });
    }

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
        qFatal("%s", msg.str().c_str());
        exit(1);
    }

    assert(dir.exists());
    return dir;
}

QUrl conf_scheduler::get_data_file_location(QString code, const QString &extension)
{
    return QUrl::fromLocalFile(get_existing_data_dir().filePath(code.append(extension))).path();
}
