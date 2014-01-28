#include "event.h"

#include <cassert>

#include <QtDebug>

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
    end_(data.endtime),
    favorite_(data.favorite)
{
    assert(parent != nullptr);
}

void event::favorite(bool status)
{
    if(favorite() == status) return;

    const auto parent_ptr = qobject_cast<cfs::conference*>(parent());
    assert(parent_ptr);
    if(parent_ptr)
    {
        try
        {
            if(status) parent_ptr->star_event(*this);
            else parent_ptr->unstar_event(*this);

            favorite_ = status;
            emit favoriteChanged(favorite_);
        }
        catch(const std::exception &e)
        {
            qDebug() << "Exception:" << e.what();
        }
    }
}
