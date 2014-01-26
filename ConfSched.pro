# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = ConfSched

CONFIG += sailfishapp
CONFIG += c++11

SOURCES += src/ConfSched.cpp \
    src/conf_scheduler.cpp \
    src/conference.cpp \
    src/sync_downloader.cpp \
    src/qt_downloader.cpp \
    src/pentabarf_parser.cpp \
    src/storage.cpp \
    src/conference_list.cpp \
    src/event.cpp \
    src/event_list_model.cpp

OTHER_FILES += qml/ConfSched.qml \
    qml/cover/CoverPage.qml \
    rpm/ConfSched.spec \
    rpm/ConfSched.yaml \
    ConfSched.desktop \
    qml/pages/ConferenceOverviewPage.qml \
    qml/pages/AddConferencePage.qml \
    qml/pages/ConferenceDetailPage.qml \
    qml/pages/EventDetailPage.qml

HEADERS += \
    src/conf_scheduler.h \
    src/conference.h \
    src/sync_downloader.h \
    src/qt_downloader.h \
    src/conference_data.h \
    src/pentabarf_parser.h \
    src/storage.h \
    src/conference_list.h \
    src/event.h \
    src/event_list_model.h

QT += network
QT += sql

DEFINES *= QT_USE_QSTRINGBUILDER
