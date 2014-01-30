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
    remote_file_(cd.remote_data),
    starttime_(cd.start),
    endtime_(cd.end)
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
    //TODO conference should use a vector internally so we do not need to convert
    std::vector<cfs::event*> evts;
    evts.reserve(events_.size());
    std::copy(std::begin(events_), std::end(events_), std::back_inserter(evts));
    return new event_list_model(std::move(evts), const_cast<conference*>(this));
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

void conference::star_event(const cfs::event &e)
{
    const auto parent_ptr = qobject_cast<cfs::conf_scheduler*>(parent());
    assert(parent_ptr);
    if(parent_ptr) parent_ptr->star_event(*this, e);
    else throw std::runtime_error("Conference has no valid parent element;");
}

void conference::unstar_event(const cfs::event &e)
{
    const auto parent_ptr = qobject_cast<cfs::conf_scheduler*>(parent());
    assert(parent_ptr);
    if(parent_ptr) parent_ptr->unstar_event(*this, e);
    else throw std::runtime_error("Conference has no valid parent element;");
}

void conference::update(bool update_remote_data, bool update_full_event)
{
    const auto parent_ptr = qobject_cast<cfs::conf_scheduler*>(parent());
    assert(parent_ptr);
    if(parent_ptr) parent_ptr->updateConference(this, update_remote_data, update_full_event);
}

void conference::unsubscribe()
{
    const auto parent_ptr = qobject_cast<cfs::conf_scheduler*>(parent());
    assert(parent_ptr);
    if(parent_ptr) parent_ptr->removeConference(this);
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
}
