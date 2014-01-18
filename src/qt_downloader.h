#ifndef CFS_QT_DOWNLOADER_H
#define CFS_QT_DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QUrl>
#include <QDir>

namespace cfs
{

class qt_downloader : public QObject
{
    Q_OBJECT

public:
    explicit qt_downloader(QObject *parent = nullptr);

signals:
    void done() const;
    void error(const QString &message) const;

public slots:
    void downloadTo(QUrl source, QUrl target);
    void finished(QNetworkReply *data);

private:
    static QDir get_existing_cache_dir();

    void start_download();

private:
    QNetworkReply *reply_;
    QNetworkAccessManager manager_;
    QNetworkDiskCache cache_;
    QUrl source_;
    QUrl target_;
};

} //namespace cfs

#endif //CFS_QT_DOWNLOADER_H
