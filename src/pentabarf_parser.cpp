#include "pentabarf_parser.h"

#include <stdexcept>
#include <cassert>
#include <iostream>

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QDate>

using cfs::detail::conference_data;
using cfs::detail::pentabarf_parser;

std::unique_ptr<conference_data> pentabarf_parser::parse(QFile &file)
{
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        std::unique_ptr<conference_data> d;
        QXmlStreamReader xml(&file);
        xml.readNext();
        qDebug() << "Opened Pentabarf file with " << xml.documentEncoding() << " encoding.";
        for(auto token = xml.readNext();
            !xml.atEnd() && !xml.hasError();
            token = xml.readNext())
        {
            static const auto CONF_TAG = "conference";
            static const auto EVENT_TAG = "day";
            if(token == QXmlStreamReader::StartElement)
            {
                if(xml.name() == CONF_TAG)
                {
                    //check that we do not overwrite existing conf_data
                    assert(!d || d->events.size() == 0);
                    d = parse_conf(xml);
                }
                else if(xml.name() == EVENT_TAG)
                {
                    assert(d); //conference data comes before event data in pentabarf xml
                    d->events += parse_events(xml);
                }
            }
        }
        if(xml.hasError())
        {
            qDebug() << "XML parsing error: " << xml.errorString();
            throw std::runtime_error("Failed to parse conference data.");
        }

        return d;
    }
    else
    {
        qDebug() << "Failed opening conf data file:" << file.errorString();
        throw std::runtime_error("Could not read conference data.");
    }

    return nullptr;
}

//methods for parsing the general conference data
std::unique_ptr<conference_data> pentabarf_parser::parse_conf(QXmlStreamReader &xml)
{
    std::unique_ptr<conference_data> d(new conference_data());
    for(auto token = xml.readNext();
        !xml.atEnd() && !xml.hasError();
        token = xml.readNext())
    {
        static const auto CONF_TAG = "conference";
        static const auto TITLE_TAG = "title";
        static const auto SUBTITLE_TAG = "subtitle";
        static const auto VENUE_TAG = "venue";
        static const auto CITY_TAG = "city";

        if(token == QXmlStreamReader::StartElement)
        {
            assert(d);
            if(xml.name() == TITLE_TAG)
            {
                d->title = parse_conf_title(xml);
            }
            else if(xml.name() == SUBTITLE_TAG)
            {
                d->subtitle = parse_conf_subtitle(xml);
            }
            else if(xml.name() == VENUE_TAG)
            {
                d->venue = parse_conf_venue(xml);
            }
            else if(xml.name() == CITY_TAG)
            {
                d->city = parse_conf_city(xml);
            }
        }
        else if(token == QXmlStreamReader::EndElement &&
                xml.name() == CONF_TAG)
        {
            qDebug() << "Conference data parsed: " <<
                        "title: " << d->title << ", " <<
                        "subtitle: " << d->subtitle << ", " <<
                        "venue: " << d->venue << ", " <<
                        "city: " << d->city;
            return d;
        }
    }

    //this is only reached when the end tag is missing. if we end here
    //xml should already be in an error state, which is handled in the
    //main parse() method, so it's safe to return probably faulty data.
    assert(xml.hasError());
    return nullptr;
}

QString pentabarf_parser::parse_conf_title(QXmlStreamReader &xml)
{
    assert(xml.isStartElement());
    return xml.readElementText();
}

QString pentabarf_parser::parse_conf_subtitle(QXmlStreamReader &xml)
{
    assert(xml.isStartElement());
    return xml.readElementText(QXmlStreamReader::SkipChildElements);
}

QString pentabarf_parser::parse_conf_venue(QXmlStreamReader &xml)
{
    assert(xml.isStartElement());
    return xml.readElementText();
}

QString pentabarf_parser::parse_conf_city(QXmlStreamReader &xml)
{
    assert(xml.isStartElement());
    return xml.readElementText();
}

//methods for parsing the conference events data
pentabarf_parser::event_list pentabarf_parser::parse_events(QXmlStreamReader &xml)
{
    event_list list;

    static const auto DAY_TAG = "day";
    if(xml.tokenType() == QXmlStreamReader::StartElement &&
       xml.name() == DAY_TAG)
    {
        static const auto DATE_ATTRIB = "date";
        assert(xml.attributes().hasAttribute(DATE_ATTRIB));
        const QDate date = QDate::fromString(xml.attributes().value(DATE_ATTRIB).toString(), "yyyy-MM-dd");
        assert(date.isValid());

        for(auto token = xml.readNext();
            !xml.atEnd() && !xml.hasError() && !(token == QXmlStreamReader::EndElement && xml.name() == DAY_TAG);
            token = xml.readNext())
        {
            if(token == QXmlStreamReader::StartElement)
            {
                static const auto ROOM_TAG = "room";
                if(xml.name() == ROOM_TAG)
                {
                    list += parse_events_per_room(xml, date);
                }
            }
        }

        qDebug() << list.size() << "events on" << date << "parsed.";
    }
    else
    {
        qWarning("No valid event day found that could be parsed.");
    }

    return list;
}

pentabarf_parser::event_list pentabarf_parser::parse_events_per_room(QXmlStreamReader &xml, const QDate &date)
{
    event_list list;

    static const auto ROOM_TAG = "room";
    if(xml.tokenType() == QXmlStreamReader::StartElement &&
       xml.name() == ROOM_TAG)
    {
        static const auto NAME_ATTRIB = "name";
        assert(xml.attributes().hasAttribute(NAME_ATTRIB));
        const QString room = xml.attributes().value(NAME_ATTRIB).toString();
        assert(room.size() > 0);

        for(auto token = xml.readNext();
            !xml.atEnd() && !xml.hasError() && !(token == QXmlStreamReader::EndElement && xml.name() == ROOM_TAG);
            token = xml.readNext())
        {
            if(token == QXmlStreamReader::StartElement)
            {
                static const auto EVENT_TAG = "event";
                if(xml.name() == EVENT_TAG)
                {
                    list += parse_single_event(xml, date, room);
                }
            }
        }

        qDebug() << list.size() << "events on" << date << "in" << room << "parsed.";
    }
    else
    {
        qWarning("No valid event room found that could be parsed.");
    }

    return list;
}

pentabarf_parser::event_data pentabarf_parser::parse_single_event(QXmlStreamReader &xml, const QDate &date, const QString &room)
{
    assert(!date.isNull() && date.isValid());
    assert(room.length() > 0);

    static const auto EVENT_TAG = "event";
    if(xml.tokenType() == QXmlStreamReader::StartElement &&
       xml.name() == EVENT_TAG)
    {
        event_data event;
        event.room = room;

        static const auto ID_ATTRIB = "id";
        assert(xml.attributes().hasAttribute(ID_ATTRIB));
        event.id = xml.attributes().value(ID_ATTRIB).toInt();

        for(auto token = xml.readNext();
            !xml.atEnd() && !xml.hasError() && !(token == QXmlStreamReader::EndElement && xml.name() == EVENT_TAG);
            token = xml.readNext())
        {
            if(token == QXmlStreamReader::StartElement)
            {
                static const auto TITLE_TAG = "title";
                static const auto SUBTITLE_TAG = "subtitle";
                static const auto TRACK_TAG = "track";
                static const auto ABSTRACT_TAG = "abstract";
                static const auto DESC_TAG = "description";
                static const auto START_TAG = "start";
                static const auto DURATION_TAG = "duration";

                if(xml.name() == TITLE_TAG) event.title = xml.readElementText();
                else if(xml.name() == SUBTITLE_TAG) event.subtitle = xml.readElementText(QXmlStreamReader::SkipChildElements);
                else if(xml.name() == TRACK_TAG) event.track = xml.readElementText();
                else if(xml.name() == ABSTRACT_TAG) event.abstract = xml.readElementText(QXmlStreamReader::SkipChildElements);
                else if(xml.name() == DESC_TAG) event.description = xml.readElementText(QXmlStreamReader::SkipChildElements);
                else if(xml.name() == START_TAG)
                {
                    const auto time = QTime::fromString(xml.readElementText(), "HH:mm");
                    assert(!time.isNull() && time.isValid());
                    event.starttime = QDateTime(date, time);
                }
                else if(xml.name() == DURATION_TAG)
                {
                    const auto time = QTime::fromString(xml.readElementText(), "HH:mm");
                    assert(!time.isNull() && time.isValid());
                    event.endtime = QDateTime(date, time);
                }
            }
        }

        assert(event.id != 0);
        assert(event.title.size() > 0);
        //assert(event.subtitle.size() > 0); optional value
        //assert(event.track.size() > 0); optional value
        //assert(event.abstract.size() > 0); optional value
        //assert(event.description.size() > 0); optional value
        assert(event.room.size() > 0);
        assert(event.starttime.isValid());
        assert(event.endtime.isValid());

        return event;
    }

    throw std::runtime_error("Could not parse event data.");
}
