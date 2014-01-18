#ifndef CFS_SYNC_DOWNLOADER_H
#define CFS_SYNC_DOWNLOADER_H

#include <memory>

#include <QUrl>
#include <QFile>

namespace cfs
{

class qt_downloader;

class sync_downloader
{
public:
    sync_downloader();
    ~sync_downloader();

    bool download_to(QUrl source, QUrl target);

private:
    std::unique_ptr<qt_downloader> impl_;
};

} //namespace cfs

#endif //CFS_SYNC_DOWNLOADER_H
