#include "conference.h"

#include <QDebug>

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

conference* conference::from_file(const QUrl &data_url, QObject *parent)
{
    qDebug() << data_url << ", " << parent;
    return new conference(parent);
}
