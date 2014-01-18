#include "conference.h"

#include <sstream>
#include <stdexcept>
#include <cassert>

#include <QDebug>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QDir>

#include "sync_downloader.h"

using cfs::conference;

conference::conference(QObject *parent) :
    QObject(parent),
    id_(0)
{
}

conference::conference(const QString &title,
                       const QString &subtitle,
                       const QString &venue,
                       const QString &city,
                       const QUrl &data_url,
                       QObject *parent) :
    QObject(parent),
    id_(0),
    title_(title),
    subtitle_(subtitle),
    venue_(venue),
    city_(city),
    data_url_(data_url)
{
}

#ifndef NDEBUG
conference::~conference()
{
    qDebug() << "conf: " << id_;
}
#endif

/*bool create_parent_if_necessary(const QUrl &local_path)
{
    const QDir path(local_path);
    if(!path.exists() && )
}*/

conference* conference::from_file(const QUrl &data_url, QObject *parent)
{
    qDebug() << data_url << ", " << parent;

    const auto &target = compute_data_location(compute_conference_code(data_url));
    qDebug() << "target: " << target;

    sync_downloader dl;
    const auto &file = dl.download_to(data_url, target);
    return new conference(parent);
}

QString conference::compute_conference_code(const QUrl &data_url)
{
    QCryptographicHash hasher(QCryptographicHash::Md5);
    hasher.addData(data_url.path().toUtf8());

    return QString::fromLocal8Bit(hasher.result().toHex());
}

QUrl conference::compute_data_location(QString code)
{
    return QUrl::fromLocalFile(get_existing_data_dir().filePath(code.append(".xml"))).path();
}

QDir conference::get_existing_data_dir()
{
    const auto &dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if(!dir.exists() && !dir.mkpath(dir.path()))
    {
        std::ostringstream msg;
        msg << "Data directory '" << dir.path().toLocal8Bit().data() << "' does not exist, " <<
               "and can not be created.";
        throw std::runtime_error(msg.str().c_str());
    }

    assert(dir.exists());
    return dir;
}
