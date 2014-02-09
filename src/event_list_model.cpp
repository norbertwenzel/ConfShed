#include "event_list_model.h"

#include <cassert>
#include <algorithm>
#include <iterator>

#include <QDebug>

using cfs::event_list_model;

event_list_model::event_list_model(QObject *parent) :
    QAbstractListModel(parent),
    data_(),
    filtered_size_(-1)
{
}

event_list_model::event_list_model(std::vector<cfs::event*> list, QObject *parent) :
    QAbstractListModel(parent),
    data_(std::move(list)),
    filtered_size_(-1)
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
    roles[ROLE_FAVORITE] = "favorite";
    roles[ROLE_WEEKDAY] = "weekday";
    return roles;
}

int event_list_model::rowCount(const QModelIndex&) const
{
    return filtered_size_ >= 0 ? filtered_size_ : data_.size();
}

QVariant event_list_model::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > rowCount())
    {
        return QVariant();
    }

    const auto &cur_event = data_[index.row()];
    assert(cur_event);
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
    case ROLE_FAVORITE:
        return cur_event->favorite();
        break;
    case ROLE_WEEKDAY:
        return get_weekday(*cur_event);
        break;

    default:
        assert(false);
        break;
    }

    return QVariant();
}

void event_list_model::sort_by(event_list_model::sort_criteria criterion,
                               Qt::SortOrder order /* = Qt::AscendingOrder */)
{
    qDebug();

    emit layoutAboutToBeChanged();

    std::sort(std::begin(data_), std::end(data_),
    [=](const cfs::event *e1, const cfs::event *e2) -> bool
    {
        if(criterion == SortTitle)
        {
            if(order == Qt::AscendingOrder)
            {
                return e1->title() < e2->title();
            }
            else
            {
                assert(order == Qt::DescendingOrder);
                return e1->title() > e2->title();
            }
        }
        else if(criterion == SortTrack)
        {
            if(order == Qt::AscendingOrder)
            {
                return e1->track() < e2->track();
            }
            else
            {
                assert(order == Qt::DescendingOrder);
                return e1->track() > e2->track();
            }
        }
        else if(criterion == SortDay)
        {
            if(order == Qt::AscendingOrder)
            {
                return e1->starttime() < e2->starttime();
            }
            else
            {
                assert(order == Qt::DescendingOrder);
                return e1->starttime() > e2->starttime();
            }
        }
        assert(false);
        return false;
    });

    changePersistentIndex(createIndex(0, 0), createIndex(rowCount() - 1, 0));
    emit layoutChanged();
}

void event_list_model::filter_by(event_list_model::filter_criteria criterion, QString the_filter)
{
    qDebug() << criterion << the_filter;

    if(criterion == FilterNone)
    {
        filter_.clear();
        beginInsertRows(QModelIndex(), rowCount(), data_.size() - rowCount());
        filtered_size_ = -1;
        emit endInsertRows();

        return;
    }

    assert(criterion != FilterNone);
    filter_ = std::move(the_filter);

    emit layoutAboutToBeChanged();

    int new_size = -1;
    if(criterion == FilterTrack)
    {
        const auto it = std::stable_partition(std::begin(data_), std::end(data_),
        [&](const cfs::event *evt){ return QString::compare(evt->track(), filter_, Qt::CaseInsensitive) == 0; });

        new_size = it != std::end(data_) ? std::distance(std::begin(data_), it) : -1;
        qDebug() << "Filtered" << new_size << "events for track" << filter_;
    }
    else if(criterion == FilterDay)
    {
        const auto it = std::stable_partition(std::begin(data_), std::end(data_),
        [&](const cfs::event *evt){ return QString::compare(get_weekday(*evt), filter_, Qt::CaseInsensitive) == 0; });

        new_size = it != std::end(data_) ? std::distance(std::begin(data_), it) : -1;
        qDebug() << "Filtered" << new_size << "events for day" << filter_;
    }
    else if(criterion == FilterCurrentTime)
    {
        const QDateTime FILTER_TIME = QDateTime::currentDateTime();
                //QDateTime::fromString("2014-02-02 16:00:00", "yyyy-MM-dd HH:mm:ss");

        const auto it = std::stable_partition(std::begin(data_), std::end(data_),
        [&](const cfs::event *evt){ return evt->starttime() > FILTER_TIME; });

        new_size = it != std::end(data_) ? std::distance(std::begin(data_), it) : -1;
        qDebug() << "Filtered" << new_size << "events with starttime >" << FILTER_TIME;
    }

    changePersistentIndex(createIndex(0, 0), createIndex(std::max(rowCount() - 1, 0), 0));
    emit layoutChanged();

    beginRemoveRows(QModelIndex(), new_size, data_.size());

    filtered_size_ = new_size;

    emit endRemoveRows();
}

QVariant event_list_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if(orientation == Qt::Horizontal)
    {
        return QString("Column %1").arg(section);
    }
    else
    {
        return QString("Row %1").arg(section);
    }
}

QList<QString> event_list_model::getTracks() const
{
    //TODO: cache result instead of recomputing

    QList<QString> tracks;
    tracks.reserve(data_.size());
    std::transform(std::begin(data_), std::end(data_), std::back_inserter(tracks),
                   [](const cfs::event *evt){ return evt->track(); });
    tracks = make_unique_set(std::move(tracks));

    qDebug() << "Returned" << tracks.length() << "tracks.";

    return tracks;
}

QList<QString> event_list_model::getRooms() const
{
    //TODO: cache result instead of recomputing

    QList<QString> rooms;
    rooms.reserve(data_.size());
    std::transform(std::begin(data_), std::end(data_), std::back_inserter(rooms),
                   [](const cfs::event *evt){ return evt->room(); });
    rooms = make_unique_set(std::move(rooms));

    qDebug() << "Returned" << rooms.length() << "rooms.";

    return rooms;
}

bool event_list_model::make_item_favorite(int index, bool favorite)
{
    qDebug() << index << favorite;

    if(index < 0 || index >= rowCount())
    {
        return false;
    }

    data_.at(index)->favorite(favorite);
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    return true;
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

QString event_list_model::get_weekday(const cfs::event &evt) const
{
    return evt.starttime().toString("dddd");
}


template<typename T>
T event_list_model::make_unique_set(T&& data) const
{
    std::sort(std::begin(data), std::end(data));
    data.erase(std::unique(std::begin(data), std::end(data)), std::end(data));

    return data;
}
