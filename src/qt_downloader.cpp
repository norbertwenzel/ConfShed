#include "qt_downloader.h"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <algorithm>

#include <QStandardPaths>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QDebug>
#include <QtGlobal>

using cfs::qt_downloader;

qt_downloader::qt_downloader(QObject *parent) :
    QObject(parent),
    reply_(nullptr)
{
    const auto &cache_dir = get_existing_cache_dir();
    qDebug() << "cache_dir: " << cache_dir << " (as string: " << cache_dir.path() << ")";

    assert(cache_dir.exists());
    cache_.setCacheDirectory(cache_dir.path());
    manager_.setCache(&cache_);

    //get informed when download is done
    connect(&manager_, &QNetworkAccessManager::finished, this, &qt_downloader::finished);
}

void qt_downloader::downloadTo(QUrl source, QUrl target)
{
    qDebug() << source << ", " << target;
    source_ = std::move(source);
    target_ = std::move(target);

    start_download();
}

void qt_downloader::start_download()
{
    qDebug();

    QNetworkRequest req(source_);
    req.setHeader(QNetworkRequest::UserAgentHeader, QString("ConfSched for Sailfish"));
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    reply_ = manager_.get(req);
}

void qt_downloader::finished(QNetworkReply *data)
{
    qDebug();

    bool no_error = true;
    if(data->error())
    {
        no_error = false;
        qCritical() << data->errorString();
        emit error(QString::fromLocal8Bit("Failed to download conference data."));
    }
    /*else
    {
        std::ostringstream msg;
        const auto &headers = data->rawHeaderList();
        std::for_each(std::begin(headers), std::end(headers),
        [&](const QByteArray &h)
        {
            msg << h.data() << ": " << data->rawHeader(h).data() << std::endl;
        });
        qDebug() << msg.str().c_str();
    }*/

    if(no_error)
    {
        const auto &content_type = data->header(QNetworkRequest::ContentTypeHeader).toString();
        const auto &redir = data->attribute(QNetworkRequest::RedirectionTargetAttribute);
        qDebug() <<  "content_type: " << content_type << "; redirection: " << redir;

        const bool is_text = content_type.startsWith("text", Qt::CaseInsensitive) || content_type.contains("xml", Qt::CaseInsensitive);
        const auto idx_left = content_type.length() - content_type.lastIndexOf('=') - 1;
        const QString charset = idx_left > 0 && idx_left < content_type.length() ? content_type.rightRef(idx_left).toString() : QString("utf-8"); //assume utf8 encoding

        QFile file(target_.toString());
        if(file.open(QFile::WriteOnly | QFile::Truncate))
        {
            if(is_text)
            {
                qDebug() << "Using QTextStream with codec " << charset << " idx: " << idx_left;
                QTextStream fstream(&file);
                fstream.setCodec(charset.toLocal8Bit().data());
                fstream.setGenerateByteOrderMark(true);
                fstream << data->readAll();
            }
            else
            {
                qDebug() << "Using QDataStream";
                QDataStream fstream(&file);
                fstream << data->readAll();
            }
        }
        else
        {
            no_error = false;
            qDebug() << "Could not write to file " << target_;
            emit error(QString::fromLocal8Bit("Failed to store the conference data."));
        }
        file.close();
        assert(QFile::exists(target_.toString()));
    }

    data->deleteLater();
    reply_ = nullptr;
    source_.clear();
    target_.clear();

    if(no_error)
    {
        emit done();
    }
}

QDir qt_downloader::get_existing_cache_dir()
{
    const auto &dir = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if(!dir.exists() && !dir.mkpath(dir.path()))
    {
        std::ostringstream msg;
        msg << "Cache directory '" << dir.path().toLocal8Bit().data() << "' does not exist, " <<
               "and can not be created.";
        throw std::runtime_error(msg.str().c_str());
    }

    assert(dir.exists());
    return dir;
}
