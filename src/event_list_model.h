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

    Q_PROPERTY(int length READ rowCount CONSTANT)

public:
    enum sort_criteria { Title, Track, Day };
    explicit event_list_model(QObject *parent = nullptr);
    explicit event_list_model(std::vector<cfs::event*> list, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Q_INVOKABLE bool make_item_favorite(int index, bool favorite);

    Q_INVOKABLE void sort_by(sort_criteria criterion, Qt::SortOrder order = Qt::AscendingOrder);

    Q_INVOKABLE cfs::event* get(int id) const;

signals:

public slots:

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
};

} //namespace cfs

#endif //CFS_EVENT_LIST_MODEL_H
