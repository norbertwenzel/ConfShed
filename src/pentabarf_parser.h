#ifndef CFS_DETAIL_PENTABARF_PARSER_H
#define CFS_DETAIL_PENTABARF_PARSER_H

#include <QString>

#include "conference_data.h"

class QFile;
class QXmlStreamReader;

namespace cfs
{
namespace detail
{

class pentabarf_parser : public conference_parser
{
public:
    conference_data parse(QFile &file);

private:
    //general methods for helping with parsing
    bool extract_text(QXmlStreamReader &xml, const QString &tag, QString &result) const;

    //methods for parsing the general conference data
    conference_data parse_conf(QXmlStreamReader &xml);
    QString parse_conf_title(QXmlStreamReader &xml);
    QString parse_conf_subtitle(QXmlStreamReader &xml);
    QString parse_conf_venue(QXmlStreamReader &xml);
    QString parse_conf_city(QXmlStreamReader &xml);

    //methods for parsing the conference events data
};

} //namespace detail
} //namespace cfs

#endif //CFS_DETAIL_PENTABARF_PARSER_H
