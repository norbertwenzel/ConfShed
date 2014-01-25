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
    title_(data.title),
    subtitle_(data.subtitle),
    track_(data.track),
    abstract_(data.abstract),
    description_(data.description),
    room_(data.room),
    persons_(data.persons),
    start_(data.starttime),
    end_(data.endtime)
{
    assert(parent != nullptr);
}
