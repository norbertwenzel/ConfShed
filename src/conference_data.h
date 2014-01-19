#ifndef CFS_DETAIL_CONFERENCE_DATA_H
#define CFS_DETAIL_CONFERENCE_DATA_H

#include <QString>

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
};

class conference_parser
{
public:
    virtual ~conference_parser(){}

    virtual conference_data parse(QFile &file) = 0;
};

} //namespace detail
} //namespace cfs

#endif // CFS_DETAIL_CONFERENCE_DATA_H
