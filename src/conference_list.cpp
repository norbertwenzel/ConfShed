#include "conference_list.h"

#include <cassert>
#include <algorithm>

#include "conference.h"

using cfs::conference_list_model;

conference_list_model::conference_list_model(QObject *parent) :
    QAbstractListModel(parent)
{
}

conference_list_model::conference_list_model(QList<cfs::conference*> list,
                                             QObject *parent) :
    QAbstractListModel(parent),
    data_(std::move(list))
{
}

QHash<int, QByteArray> conference_list_model::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ROLE_ID] = "conf_id";
    roles[ROLE_TITLE] = "title";
    roles[ROLE_SUBTITLE] = "subtitle";
    roles[ROLE_VENUE] = "venue";
    roles[ROLE_CITY] = "city";
    return roles;
}

int conference_list_model::rowCount(const QModelIndex&) const
{
    return data_.count();
}

QVariant conference_list_model::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > rowCount())
    {
        return QVariant();
    }

    const auto &cur_conf = data_[index.row()];
    switch(role)
    {
    case ROLE_ID:
        return cur_conf->conf_id();
        break;
    case ROLE_TITLE:
        return cur_conf->title();
        break;
    case ROLE_SUBTITLE:
        return cur_conf->subtitle();
        break;
    case ROLE_VENUE:
        return cur_conf->venue();
        break;
    case ROLE_CITY:
        return cur_conf->city();
        break;

    default:
        assert(false);
        break;
    }

    return QVariant();
}

cfs::conference* conference_list_model::get(const int id) const
{
    const auto result_iter = std::find_if(std::begin(data_), std::end(data_),
                            [=](decltype(*std::begin(data_)) &conf)
                            {
                                return conf->conf_id() == id;
                            });
    return result_iter != std::end(data_) ? *result_iter : nullptr;
}
