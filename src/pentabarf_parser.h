#ifndef CFS_DETAIL_PENTABARF_PARSER_H
#define CFS_DETAIL_PENTABARF_PARSER_H

#include "conference_data.h"

class QFile;

namespace cfs
{
namespace detail
{

class pentabarf_parser : public conference_parser
{
public:
    conference_data parse(QFile &file);
};

} //namespace detail
} //namespace cfs

#endif //CFS_DETAIL_PENTABARF_PARSER_H
