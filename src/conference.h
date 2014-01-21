#ifndef CFS_CONFERENCE_H
#define CFS_CONFERENCE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QSharedPointer>
#include <QDir>

#include "conference_data.h"

namespace cfs
{

class conference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int conf_id READ conf_id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString subtitle READ subtitle CONSTANT)
    Q_PROPERTY(QString venue READ venue CONSTANT)
    Q_PROPERTY(QString city READ city CONSTANT)
    Q_PROPERTY(QString code READ code CONSTANT)
    Q_PROPERTY(QUrl remote_file READ remote_file CONSTANT)

public:
    explicit conference(QObject *parent = nullptr);
    explicit conference(const detail::conference_data &cd,
                        QObject *parent = nullptr);
#ifndef NDEBUG
    ~conference();
#endif

    int conf_id() const { return id_; }
    QString title() const { return title_; }
    QString subtitle() const { return subtitle_; }
    QString venue() const { return venue_; }
    QString city() const { return city_; }
    QString code() const { return code_; }
    QUrl remote_file() const { return remote_file_; }

    static QString compute_code(const QUrl &remote_data_url);

signals:

public slots:

private:
    static const int INVALID_CONFERENCE_ID = 0;

    int id_;
    QString title_;
    QString subtitle_;
    QString venue_;
    QString city_;
    QString code_;
    QUrl remote_file_;
};

} //namespace cfs

#endif //CFS_CONFERENCE_H
