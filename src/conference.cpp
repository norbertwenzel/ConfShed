#include "conference.h"

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <algorithm>

#include <QDebug>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "sync_downloader.h"
#include "conference_data.h"
#include "pentabarf_parser.h"

using cfs::conference;

conference::conference(QObject *parent) :
    QObject(parent),
    id_(INVALID_CONFERENCE_ID)
{
}

conference::conference(const QString &title,
                       const QString &subtitle,
                       const QString &venue,
                       const QString &city,
                       const QUrl &remote_data_url,
                       QObject *parent) :
    QObject(parent),
    id_(INVALID_CONFERENCE_ID),
    title_(title),
    subtitle_(subtitle),
    venue_(venue),
    city_(city),
    code_(compute_conference_code(remote_data_url))
{
    id_ = store();
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

    const auto &code = compute_conference_code(data_url);
    const auto &target = compute_file_location(code, DATA_FILE_EXT);
    qDebug() << "target: " << target << ", code: " << code;

    sync_downloader dl;
    if(dl.download_to(data_url, target))
    {
        const auto parser = std::unique_ptr<detail::conference_parser>(new detail::pentabarf_parser());

        QFile file(target.path());
        if(file.exists())
        {
            const auto &data = parser->parse(file);
            return new conference(data.title, data.subtitle, data.venue, data.city, data_url, parent);
        }
        else
        {
            throw std::runtime_error("Failed to read conference data.");
        }
    }
    else
    {
        throw std::runtime_error("Failed to create new conference.");
    }

    return new conference(parent);
}

QString conference::compute_conference_code(const QUrl &remote_data_url)
{
    QCryptographicHash hasher(QCryptographicHash::Md5);
    hasher.addData(remote_data_url.path().toUtf8());

    return QString::fromLocal8Bit(hasher.result().toHex());
}

QUrl conference::compute_file_location(QString code, const QString &extension)
{
    return QUrl::fromLocalFile(get_existing_data_dir().filePath(code.append(extension))).path();
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

int conference::store()
{
    /*const auto db_drivers = QSqlDatabase::drivers();
    std::for_each(std::begin(db_drivers), std::end(db_drivers),
    [](decltype(*std::begin(db_drivers)) &db)
    {
        qDebug() << db;
    });*/

    static const auto db_driver = "QSQLITE";

    if(!QSqlDatabase::isDriverAvailable(db_driver))
    {
        std::ostringstream msg;
        msg << "Missing driver for database " << db_driver;
        qFatal(msg.str().c_str());
        throw std::runtime_error("Database driver missing.");
    }

    auto db = QSqlDatabase::addDatabase(db_driver);
    db.setDatabaseName(compute_file_location("data", DB_FILE_EXT).toString());
    auto is_open = db.open();

    if(is_open)
    {
        qDebug() << "Database " << db.databaseName() << " is open";

        auto db_res = db.exec("CREATE TABLE IF NOT EXISTS confs(Id INTEGER PRIMARY KEY, Title TEXT, Venue TEXT, City TEXT, Code TEXT UNIQUE)");
        if(db_res.lastError().type() == QSqlError::NoError)
        {
            QSqlQuery query;
            query.prepare("INSERT OR ABORT INTO confs (Title, Venue, City, Code) VALUES (:title, :venue, :city, :code)");
            query.bindValue(":title", title_);
            query.bindValue(":venue", venue_);
            query.bindValue(":city", city_);
            query.bindValue(":code", code_);

            if(!query.exec()) throw std::runtime_error(query.lastError().text().toLocal8Bit().data());
            //return query.lastInsertId().toInt();

            db_res = db.exec("SELECT * FROM confs");
            while(db_res.next())
            {
                qDebug() << db_res.record();
            }
        }
        else
        {
            qDebug() << db_res.lastError();
        }

#ifndef NDEBUG
        //delete the database in debug mode, for easier testing
        db.exec("DROP TABLE confs");
#endif
    }
    else
    {
        throw std::runtime_error("Failed to access the database.");
    }

    throw std::runtime_error("Unknown database error.");
}
