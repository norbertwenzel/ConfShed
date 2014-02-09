#ifndef CFS_EVENT_LIST_MODEL_H
#define CFS_EVENT_LIST_MODEL_H

#include <vector>

#include <QAbstractListModel>

#include "event.h"

namespace cfs
{

class event_list_model : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(sort_criteria)
    Q_ENUMS(filter_criteria)

    Q_PROPERTY(int length READ rowCount CONSTANT)
    Q_PROPERTY(QList<QString> tracks READ getTracks NOTIFY tracksChanged)
    Q_PROPERTY(QList<QString> rooms READ getRooms NOTIFY roomsChanged)

public:
    enum sort_criteria { SortTitle, SortTrack, SortDay };
    enum filter_criteria { FilterNone, FilterTrack, FilterDay, FilterCurrentTime };
    explicit event_list_model(QObject *parent = nullptr);
    explicit event_list_model(std::vector<cfs::event*> list, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QList<QString> getTracks() const;
    QList<QString> getRooms() const;

    Q_INVOKABLE bool make_item_favorite(int index, bool favorite);

    Q_INVOKABLE void sort_by(sort_criteria criterion, Qt::SortOrder order = Qt::AscendingOrder);
    Q_INVOKABLE void filter_by(filter_criteria criterion, QString the_filter);

    Q_INVOKABLE cfs::event* get(int id) const;

signals:
    void tracksChanged() const;
    void roomsChanged() const;

public slots:

private:
    QString get_weekday(const cfs::event &evt) const;

    template<typename T>
    T make_unique_set(T&& data) const;

private:
    enum event_roles
    {
        ROLE_ID = Qt::UserRole + 1,
        ROLE_TITLE,
        ROLE_SUBTITLE,
        ROLE_TRACK,
        ROLE_ABSTRACT,
        ROLE_DESCRIPTION,
        ROLE_ROOM,
        ROLE_PERSONS,
        ROLE_STARTTIME,
        ROLE_ENDTIME,
        ROLE_FAVORITE,
        ROLE_WEEKDAY
    };

    std::vector<cfs::event*> data_;
    int filtered_size_;
    QString filter_;
};

} //namespace cfs

#endif //CFS_EVENT_LIST_MODEL_H
