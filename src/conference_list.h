#ifndef CFS_CONFERENCE_LIST_H
#define CFS_CONFERENCE_LIST_H

#include <QAbstractListModel>
#include <QList>

#include "conference.h"

namespace cfs
{

class conference_list_model : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int length READ rowCount CONSTANT)

public:
    explicit conference_list_model(QObject *parent = nullptr);
    explicit conference_list_model(QList<conference*> list, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Q_INVOKABLE cfs::conference* get(int id) const;

signals:

public slots:

private:
    enum conf_roles
    {
        ROLE_ID = Qt::UserRole + 1,
        ROLE_TITLE,
        ROLE_SUBTITLE,
        ROLE_VENUE,
        ROLE_CITY
    };

    QList<conference*> data_;
};

} // namespace cfs

#endif // CFS_CONFERENCE_LIST_H
