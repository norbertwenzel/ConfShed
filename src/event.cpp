#include "event.h"

#include <cassert>

#include "conference.h"

using cfs::event;

event::event(QObject *parent) :
    QObject(parent)
{
}

event::event(const cfs::detail::conference_data::event_data &data,
             cfs::conference *parent) :
    QObject(parent),
    id_(data.id),
    title_(data.title.trimmed()),
    subtitle_(data.subtitle.trimmed()),
    track_(data.track.trimmed()),
    abstract_(data.abstract.trimmed()),
    description_(data.description.trimmed()),
    room_(data.room.trimmed()),
    persons_(data.persons),
    start_(data.starttime),
    end_(data.endtime)
{
    assert(parent != nullptr);
}
