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

        QSqlQuery query(db_);
        query.prepare("INSERT OR REPLACE INTO confs (Title, Subtitle, Venue, City, Code, Url) VALUES (:title, :subtitle, :venue, :city, :code, :url)");
        query.bindValue(":title", d.title);
        query.bindValue(":subtitle", d.subtitle);
        query.bindValue(":venue", d.venue);
        query.bindValue(":city", d.city);
        query.bindValue(":code", d.code);
        query.bindValue(":url", d.remote_data);

        if(!query.exec()) throw std::runtime_error(query.lastError().text().toLocal8Bit().data());
        return query.lastInsertId().toInt();
    }

    void delete_conference(int conf_id)
    {
        assert(db_.open());

        QSqlQuery query(db_);
        query.prepare("DELETE FROM confs WHERE Id = :id");
        query.bindValue(":id", conf_id);

        if(!query.exec()) throw std::runtime_error(query.lastError().text().toLocal8Bit().data());
        if(query.numRowsAffected() != 1)
        {
            assert(query.numRowsAffected() == 0); //it should be impossible to delete two items with the same id
            throw std::invalid_argument("Tried to delete nonexistent conference.");
        }
    }

    std::vector<conference_data> get_conferences()
    {
        assert(db_.open());

        QSqlQuery query(db_);
        query.prepare("SELECT * FROM confs");
        if(!query.exec()) throw std::runtime_error(query.lastError().text().toLocal8Bit().data());

        enum { ID, TITLE, VENUE, CITY, CODE, URL, SUBTITLE };
        std::vector<conference_data> results;
        while(query.next())
        {
            conference_data d;
            d.id = query.value(ID).toInt();
            d.title = query.value(TITLE).toString();
            d.subtitle = query.value(SUBTITLE).toString();
            d.venue = query.value(VENUE).toString();
            d.city = query.value(CITY).toString();
            d.code = query.value(CODE).toString();
            d.remote_data = query.value(URL).toString();

            results.push_back(std::move(d));
        }

        return results;
    }

private:
    void create_db_if_necessary()
    {
        assert(db_.open());

        auto res = db_.exec("CREATE TABLE IF NOT EXISTS confs(Id INTEGER PRIMARY KEY, Title TEXT, Venue TEXT, City TEXT, Code TEXT UNIQUE, Url TEXT, Subtitle TEXT)");
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
    return impl_->get_conferences();
}

int storage::add_or_update_conference(const conference_data &d)
{
    assert(impl_);
    return impl_->add_or_update_conference(d);
}

void storage::delete_conference(int conf_id)
{
    assert(impl_);
    impl_->delete_conference(conf_id);
}
