#include "conference.h"

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <vector>
#include <algorithm>

#include <QDebug>
#include <QCryptographicHash>

#include "sync_downloader.h"
#include "conference_data.h"
#include "pentabarf_parser.h"
#include "event.h"
#include "conf_scheduler.h"
#include "event_list_model.h"

using cfs::conference;

struct conference::cache
{
    cfs::event_list_model *current_view;

    cfs::event_list_model *events_unsorted;
    cfs::event_list_model *events_by_track;
};

conference::conference(QObject *parent) :
    QObject(parent),
    id_(INVALID_CONFERENCE_ID)
{
}

conference::conference(const detail::conference_data &cd,
                       cfs::conf_scheduler *parent) :
    QObject(parent),
    id_(cd.id),
    title_(cd.title.trimmed()),
    subtitle_(cd.subtitle.trimmed()),
    venue_(cd.venue.trimmed()),
    city_(cd.city.trimmed()),
    code_(cd.code),
    remote_file_(cd.remote_data)
{
    assert(cd.code == compute_code(cd.remote_data));
    create_events(cd.events);
}

conference::~conference()
{
    //empty dtor because of forward declared cache
    qDebug() << "conf: " << id_;
}

cfs::event_list_model *conference::events() const
{
    qDebug() << "cache = " << static_cast<bool>(cache_);
    assert(cache_);
    return cache_ ? cache_->current_view : nullptr;
}

QString conference::compute_code(const QUrl &remote_data_url)
{
    QCryptographicHash hasher(QCryptographicHash::Md5);
    hasher.addData(remote_data_url.path().toUtf8());

    return QString::fromLocal8Bit(hasher.result().toHex());
}

void conference::update_data(const cfs::detail::conference_data &cd)
{
    if(cd.id != conf_id())
    {
        std::ostringstream msg;
        msg << "Tried updating conference " << conf_id() << " with data for " << cd.id << ".";
        throw std::runtime_error(msg.str().c_str());
    }
    if(cd.code != compute_code(cd.remote_data))
    {
        throw std::runtime_error("Invalid conference data.");
    }

    assert(cd.id == conf_id());
    assert(cd.code == compute_code(cd.remote_data));

    title_ = cd.title;
    subtitle_ = cd.subtitle;
    venue_ = cd.venue;
    city_ = cd.city;
    code_ = cd.code;
    remote_file_ = cd.remote_data;

    create_events(cd.events);
}

void conference::update()
{
    const auto parent_ptr = qobject_cast<cfs::conf_scheduler*>(parent());
    assert(parent_ptr);
    if(parent_ptr) parent_ptr->updateConference(this);
}

void conference::sort_events()
{
    qDebug() << "cache =" << static_cast<bool>(cache_);
    assert(cache_);

    std::vector<cfs::event*> vec;
    vec.reserve(events_.size());
    std::copy(std::begin(events_), std::end(events_),
              std::back_inserter(vec));

    std::sort(std::begin(vec), std::end(vec),
    [](cfs::event *e1, cfs::event *e2) -> bool
    {
        if(e1->track() == e2->track())
        {
            return e1->starttime() < e2->starttime();
        }
        else
        {
            return e1->track() < e2->track();
        }
    });

    //copy the data into the view
    cache_->events_by_track = new event_list_model(std::move(vec), this);
    cache_->current_view = cache_->events_by_track;

    eventsChanged();
}

void conference::filter_events()
{

}

void conference::create_events(const QList<cfs::detail::conference_data::event_data> &ed)
{
    const auto parent_ptr = this;
    std::transform(std::begin(ed), std::end(ed), std::back_inserter(events_),
    [&](decltype(*std::begin(ed)) &data)
    {
        return new cfs::event(data, parent_ptr);
    });

    qDebug() << events_.size() << "events created.";

    if(!cache_) cache_.reset(new cache());

    //copy the data into the view
    std::vector<cfs::event*> cur;
    cur.reserve(events_.size());
    std::copy(std::begin(events_), std::end(events_),
              std::back_inserter(cur));

    cache_->events_unsorted = new event_list_model(std::move(cur), this);
    cache_->current_view = cache_->events_unsorted;
}
