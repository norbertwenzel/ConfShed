#include "storage.h"

#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <sstream>

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>

using cfs::detail::conference_data;
using cfs::detail::storage;

class storage::impl
{
public:
    impl(const QString &storage_identifier)
    {
        qDebug() << storage_identifier;

#ifndef NDEBUG
        {
            const auto db_drivers = QSqlDatabase::drivers();
            std::for_each(std::begin(db_drivers), std::end(db_drivers),
            [](decltype(*std::begin(db_drivers)) &db)
            {
                qDebug() << db;
            });
        }
#endif

        static const auto DB_DRIVER = "QSQLITE";
        if(!QSqlDatabase::isDriverAvailable(DB_DRIVER))
        {
            std::ostringstream msg;
            msg << "Missing driver for database " << DB_DRIVER;
            throw std::runtime_error("Database driver missing.");
        }

        db_ = QSqlDatabase::addDatabase(DB_DRIVER);
        db_.setDatabaseName(storage_identifier);
        assert(db_.open());

        create_db_if_necessary();
    }

public:
    int get_num_conferences()
    {
        assert(db_.open());
        auto res = db_.exec("SELECT Count(Id) FROM confs");
        if(res.lastError().type() != QSqlError::NoError)
        {
            throw std::runtime_error(res.lastError().text().toLocal8Bit().data());
        }
        if(res.first())
        {
            assert(!res.isNull(0) && res.isValid());
            return res.record().field(0).value().toInt();
        }
        assert(!res.next());

        throw std::runtime_error("Failed to read from database.");
    }

    int add_or_update_conference(const conference_data &d)
    {
        assert(db_.open());

        QSqlQuery query;
        query.prepare("INSERT OR REPLACE INTO confs (Title, Venue, City, Code, Url) VALUES (:title, :venue, :city, :code, :url)");
        query.bindValue(":title", d.title);
        query.bindValue(":venue", d.venue);
        query.bindValue(":city", d.city);
        query.bindValue(":code", d.code);
        query.bindValue(":url", d.remote_data);

        if(!query.exec()) throw std::runtime_error(query.lastError().text().toLocal8Bit().data());
        return query.lastInsertId().toInt();
    }

private:
    void create_db_if_necessary()
    {
        assert(db_.open());

        auto res = db_.exec("CREATE TABLE IF NOT EXISTS confs(Id INTEGER PRIMARY KEY, Title TEXT, Venue TEXT, City TEXT, Code TEXT UNIQUE, Url TEXT)");
        //TODO: throw exception, but check where that exception is caught
        assert(res.lastError().type() == QSqlError::NoError);
    }

private:
    QSqlDatabase db_;
};

storage::storage(const QString &storage_identifier) :
    impl_(new impl(storage_identifier))
{
    assert(impl_);
}

storage::~storage()
{
    //empty dtor because of fwd declared impl
}

int storage::get_num_conferences() const
{
    assert(impl_);
    return impl_->get_num_conferences();
}

std::vector<conference_data> storage::get_conferences() const
{
    assert(impl_);
    return std::vector<conference_data>();
}

int storage::add_or_update_conference(const conference_data &d)
{
    assert(impl_);
    return impl_->add_or_update_conference(d);
}
