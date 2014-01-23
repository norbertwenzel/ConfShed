#ifndef CFS_DETAIL_CONFERENCE_DATA_H
#define CFS_DETAIL_CONFERENCE_DATA_H

#include <memory>

#include <QString>
#include <QList>
#include <QDateTime>
#include <QUrl>

class QFile;

namespace cfs
{
namespace detail
{

struct conference_data
{
    QString title;
    QString subtitle;
    QString venue;
    QString city;

    //needed for db storage
    int id;
    QString code;
    QUrl remote_data;

    struct event_data
    {
        int id;
        QString title;
        QString subtitle;
        QString track;
        QString abstract;
        QString description;
        QString room;
        QList<QString> speaker;
        QDateTime starttime;
        QDateTime endtime;
    };
    QList<event_data> events;
};

class conference_parser
{
public:
    virtual ~conference_parser(){}

    virtual std::unique_ptr<conference_data> parse(QFile &file) = 0;
};

} //namespace detail
} //namespace cfs

#endif // CFS_DETAIL_CONFERENCE_DATA_H
