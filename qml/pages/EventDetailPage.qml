import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    property ConferenceEvent conf_event: null

    Component.onCompleted: {
        console.log(typeof conf_event);
        console.log("conf_event = " + (conf_event !== null ? "true" : "null"));
    }

    SilicaFlickable {
        id: eventDetailView
        //contentHeight: confData.height

        anchors.fill: parent

        PullDownMenu {
        }

        Connections {
            target: conf_sched
            onError: {
                console.error(message);
            }
        }

        VerticalScrollDecorator {}

        Column {
            width: parent.width

            PageHeader {
                title: "Event details"
            }
            Label {
                text: conf_event != null ? conf_event.title : ""
            }
            Label {
                text: conf_event != null ? conf_event.subtitle : ""
            }
            Label {
                text: conf_event != null ? conf_event.room : ""
            }
            Label {
                text: conf_event != null ? conf_event.persons : ""
            }
            Label {
                text: conf_event != null ? conf_event.starttime + " - " + conf_event.endtime : ""
            }
            Label {
                text: conf_event != null ? conf_event.abstract : ""
            }
            Label {
                text: conf_event != null ? conf_event.description : ""
            }
        }
    }
}
