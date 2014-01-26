import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    property Conference conf: null

    function show_event_list() {
        console.log("show_event_list()");
    }

    Component.onCompleted: {
        if(conf != null) {
            conf.eventsUpdated.connect(show_event_list);
            conf.update();
        }
    }

    SilicaFlickable {
        id: confEventList
        contentHeight: confData.height

        anchors.fill: parent

        PullDownMenu {

        }

        ViewPlaceholder {
            enabled: confEventList.count === 0
            text: "No conferences configured"
            hintText: "Pull down to add a conference"
        }

        Connections {
            target: conf_sched
            onError: {
                console.error(message);
            }
        }

        VerticalScrollDecorator {}

        Column {
            id: confData
            width: parent.width

            PageHeader {
                //title: conf != null ? conf.title : "Conference"
                title: "Conference"
            }
            SectionHeader {
                text: conf != null ? conf.title : ""
            }
            Label {
                text: conf != null ? conf.title : ""
            }
            Label {
                text: conf != null ? conf.subtitle : ""
            }
            Label {
                text: conf != null ? conf.venue : ""
            }
            Label {
                text: conf != null ? conf.city : ""
            }
        }
    }
}
