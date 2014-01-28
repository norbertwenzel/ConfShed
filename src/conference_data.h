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
    QDateTime start;
    QDateTime end;

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
        QList<QString> persons;
        QDateTime starttime;
        QDateTime endtime;
        bool favorite;
    };
    QList<event_data> events;
};

class conference_parser
{
public:
    virtual ~conference_parser(){}

    enum parsing_options
    {
        PARSE_COMPLETE,
        PARSE_WITHOUT_EVENTS
    };
    virtual std::unique_ptr<conference_data> parse(QFile &file, parsing_options p = PARSE_COMPLETE) = 0;
};

} //namespace detail
} //namespace cfs

#endif // CFS_DETAIL_CONFERENCE_DATA_H
