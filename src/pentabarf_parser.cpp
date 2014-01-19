#include "pentabarf_parser.h"

#include <stdexcept>
#include <cassert>

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

using cfs::detail::conference_data;
using cfs::detail::pentabarf_parser;

conference_data pentabarf_parser::parse(QFile &file)
{
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        conference_data d;

        QXmlStreamReader xml(&file);
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
                    d = parse_conf(xml);
                }
                else if(xml.name() == EVENT_TAG)
                {
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
}

//general methods for helping with parsing
bool pentabarf_parser::extract_text(QXmlStreamReader &xml, const QString &tag, QString &result) const
{
    if(xml.name() == tag)
    {
        xml.readNext();
        assert(!xml.hasError());
        if(xml.tokenType() == QXmlStreamReader::Characters)
        {
            result = xml.text().toString();
            return true;
        }
    }
    return false;
}

//methods for parsing the general conference data
conference_data pentabarf_parser::parse_conf(QXmlStreamReader &xml)
{
    conference_data d;

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
            if(xml.name() == TITLE_TAG)
            {
                d.title = parse_conf_title(xml);
            }
            else if(xml.name() == SUBTITLE_TAG)
            {
                d.subtitle = parse_conf_subtitle(xml);
            }
            else if(xml.name() == VENUE_TAG)
            {
                d.venue = parse_conf_venue(xml);
            }
            else if(xml.name() == CITY_TAG)
            {
                d.city = parse_conf_city(xml);
            }
        }
        else if(token == QXmlStreamReader::EndElement &&
                xml.name() == CONF_TAG)
        {
            return d;
        }
    }

    //this is only reached when the end tag is missing. if we end here
    //xml should already be in an error state, which is handled in the
    //main parse() method, so it's safe to return probably faulty data.
    assert(xml.hasError());
    return d;
}

QString pentabarf_parser::parse_conf_title(QXmlStreamReader &xml)
{
    QString title;
    extract_text(xml, "title", title);
    return title;
}

QString pentabarf_parser::parse_conf_subtitle(QXmlStreamReader &xml)
{
    QString subtitle;
    extract_text(xml, "subtitle", subtitle);
    return subtitle;
}

QString pentabarf_parser::parse_conf_venue(QXmlStreamReader &xml)
{
    QString venue;
    extract_text(xml, "venue", venue);
    return venue;
}

QString pentabarf_parser::parse_conf_city(QXmlStreamReader &xml)
{
    QString city;
    extract_text(xml, "city", city);
    return city;
}

//methods for parsing the conference events data
