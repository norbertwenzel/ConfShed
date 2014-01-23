#ifndef CFS_DETAIL_STORAGE_H
#define CFS_DETAIL_STORAGE_H

#include <vector>
#include <memory>

#include "conference_data.h"

class QString;

namespace cfs
{
namespace detail
{

class storage
{
public:
    explicit storage(const QString &storage_identifier);
    ~storage();

    int get_num_conferences() const;
    std::vector<conference_data> get_conferences() const;

    int add_or_update_conference(const conference_data &d);
    void delete_conference(int conf_id);

private:
    class impl;
    std::unique_ptr<impl> impl_;
};

} //namespace detail
} //namespace cfs

#endif //CFS_DETAIL_STORAGE_H
