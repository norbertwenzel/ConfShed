#include "pentabarf_parser.h"

#include <QFile>

using cfs::detail::conference_data;
using cfs::detail::pentabarf_parser;

conference_data pentabarf_parser::parse(QFile &file)
{
    conference_data d;
    d.title = "FOSDEM";
    d.subtitle = "subfosdem";
    d.venue = "ULB";
    d.city = "Bruxelles";

    return d;
}
