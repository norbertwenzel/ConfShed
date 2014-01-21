#include "conference.h"

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <algorithm>

#include <QDebug>
#include <QCryptographicHash>

#include "sync_downloader.h"
#include "conference_data.h"
#include "pentabarf_parser.h"

using cfs::conference;

conference::conference(QObject *parent) :
    QObject(parent),
    id_(INVALID_CONFERENCE_ID)
{
}

conference::conference(const detail::conference_data &cd,
                       QObject *parent) :
    QObject(parent),
    id_(cd.id),
    title_(cd.title),
    subtitle_(cd.subtitle),
    venue_(cd.venue),
    city_(cd.city),
    code_(cd.code),
    remote_file_(cd.remote_data)
{
    assert(cd.code == compute_code(cd.remote_data));
}

#ifndef NDEBUG
conference::~conference()
{
    qDebug() << "conf: " << id_;
}
#endif

QString conference::compute_code(const QUrl &remote_data_url)
{
    QCryptographicHash hasher(QCryptographicHash::Md5);
    hasher.addData(remote_data_url.path().toUtf8());

    return QString::fromLocal8Bit(hasher.result().toHex());
}
