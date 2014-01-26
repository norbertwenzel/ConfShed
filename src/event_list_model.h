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

    Q_PROPERTY(int length READ rowCount CONSTANT)

public:
    explicit event_list_model(QObject *parent = nullptr);
    explicit event_list_model(std::vector<cfs::event*> list, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

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
        ROLE_ENDTIME
    };

    std::vector<cfs::event*> data_;
};

} //namespace cfs

#endif //CFS_EVENT_LIST_MODEL_H
