#ifndef CFS_CONFERENCE_H
#define CFS_CONFERENCE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QSharedPointer>
#include <QDir>

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
    Q_PROPERTY(QUrl data_url READ data_url CONSTANT)

public:
    explicit conference(QObject *parent = nullptr);
#ifndef NDEBUG
    ~conference();
#endif

    static conference* from_file(const QUrl &data_url,
                                 QObject *parent);

    int conf_id() const { return id_; }
    QString title() const { return title_; }
    QString subtitle() const { return subtitle_; }
    QString venue() const { return venue_; }
    QString city() const { return city_; }
    QUrl data_url() const { return data_url_; }

signals:

public slots:

private:
    conference(const QString &title,
               const QString &subtitle,
               const QString &venue,
               const QString &city,
               const QUrl &data_url,
               QObject *parent = nullptr);

    static QString compute_conference_code(const QUrl &data_url);
    static QUrl compute_data_location(QString code);
    static QDir get_existing_data_dir();

private:
    int id_;
    QString title_;
    QString subtitle_;
    QString venue_;
    QString city_;
    QUrl data_url_;
};

} //namespace cfs

#endif //CFS_CONFERENCE_H
