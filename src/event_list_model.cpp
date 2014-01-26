#include "event_list_model.h"

#include <cassert>
#include <algorithm>

using cfs::event_list_model;

event_list_model::event_list_model(QObject *parent) :
    QAbstractListModel(parent)
{
}

event_list_model::event_list_model(std::vector<cfs::event*> list, QObject *parent) :
    QAbstractListModel(parent),
    data_(std::move(list))
{
}

QHash<int, QByteArray> event_list_model::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ROLE_ID] = "event_id";
    roles[ROLE_TITLE] = "title";
    roles[ROLE_SUBTITLE] = "subtitle";
    roles[ROLE_TRACK] = "track";
    roles[ROLE_ABSTRACT] = "abstract";
    roles[ROLE_DESCRIPTION] = "description";
    roles[ROLE_ROOM] = "room";
    roles[ROLE_PERSONS] = "persons";
    roles[ROLE_STARTTIME] = "starttime";
    roles[ROLE_ENDTIME] = "endttime";
    return roles;
}

int event_list_model::rowCount(const QModelIndex&) const
{
    return data_.size();
}

QVariant event_list_model::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > rowCount())
    {
        return QVariant();
    }

    const auto &cur_event = data_[index.row()];
    switch(role)
    {
    case ROLE_ID:
        return cur_event->event_id();
        break;
    case ROLE_TITLE:
        return cur_event->title();
        break;
    case ROLE_SUBTITLE:
        return cur_event->subtitle();
        break;
    case ROLE_TRACK:
        return cur_event->track();
        break;
    case ROLE_ABSTRACT:
        return cur_event->abstract();
        break;
    case ROLE_DESCRIPTION:
        return cur_event->description();
        break;
    case ROLE_ROOM:
        return cur_event->room();
        break;
    case ROLE_PERSONS:
        return QString("Persons"); //cur_event->persons(); //TODO: concatenate to one string
        break;
    case ROLE_STARTTIME:
        return cur_event->starttime();
        break;
    case ROLE_ENDTIME:
        return cur_event->endtime();
        break;

    default:
        assert(false);
        break;
    }

    return QVariant();
}

cfs::event* event_list_model::get(int id) const
{
    const auto result_iter = std::find_if(std::begin(data_), std::end(data_),
                            [=](decltype(*std::begin(data_)) &ev)
                            {
                                return ev->event_id() == id;
                            });
    return result_iter != std::end(data_) ? *result_iter : nullptr;
}
