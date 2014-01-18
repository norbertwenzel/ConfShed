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
    src/conference.cpp

OTHER_FILES += qml/ConfSched.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    rpm/ConfSched.spec \
    rpm/ConfSched.yaml \
    ConfSched.desktop \
    qml/pages/add_conference_page.qml

HEADERS += \
    src/conf_scheduler.h \
    src/conference.h

