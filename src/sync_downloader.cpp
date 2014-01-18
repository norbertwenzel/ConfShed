#include "sync_downloader.h"

#include <stdexcept>

#include <QEventLoop>
#include <QDebug>

#include "qt_downloader.h"


using cfs::sync_downloader;

sync_downloader::sync_downloader() :
    impl_(new qt_downloader())
{
}

sync_downloader::~sync_downloader()
{
    //empty dtor for unique_ptr to fwd declared class
}

bool sync_downloader::download_to(QUrl source, QUrl target)
{
    QString error_message;

    QEventLoop loop;
    QObject::connect(impl_.get(), &qt_downloader::done, &loop, &QEventLoop::quit);
    QObject::connect(impl_.get(), &qt_downloader::error, [&](const QString &msg) { error_message = msg; loop.exit(1); });

    impl_->downloadTo(std::move(source), std::move(target));
    if(loop.exec() != 0)
    {
        qDebug() << "EventLoop returned error: " << error_message;
        throw std::runtime_error(error_message.toLocal8Bit().data());
        return false;
    }

    return true;
}
