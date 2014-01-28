#ifndef CFS_DETAIL_PENTABARF_PARSER_H
#define CFS_DETAIL_PENTABARF_PARSER_H

#include <memory>

#include <QString>
#include <QList>
#include <QDateTime>

#include "conference_data.h"

class QFile;
class QXmlStreamReader;
class QDate;

namespace cfs
{
namespace detail
{

class conference_data;

class pentabarf_parser : public conference_parser
{
public:
    std::unique_ptr<conference_data> parse(QFile &file, conference_parser::parsing_options p);

private:
    //general methods for helping with parsing
    bool extract_text(QXmlStreamReader &xml, const QString &tag, QString &result) const;

    //methods for parsing the general conference data
    std::unique_ptr<conference_data> parse_conf(QXmlStreamReader &xml);
    QString parse_conf_title(QXmlStreamReader &xml);
    QString parse_conf_subtitle(QXmlStreamReader &xml);
    QString parse_conf_venue(QXmlStreamReader &xml);
    QString parse_conf_city(QXmlStreamReader &xml);
    QDateTime parse_conf_start(QXmlStreamReader &xml);
    QDateTime parse_conf_end(QXmlStreamReader &xml);

    //methods for parsing the conference events data
    typedef conference_data::event_data event_data;
    typedef QList<event_data> event_list;
    event_list parse_events(QXmlStreamReader &xml);
    event_list parse_events_per_room(QXmlStreamReader &xml, const QDate &date);
    event_data parse_single_event(QXmlStreamReader &xml, const QDate &date, const QString &room);
};

} //namespace detail
} //namespace cfs

#endif //CFS_DETAIL_PENTABARF_PARSER_H
