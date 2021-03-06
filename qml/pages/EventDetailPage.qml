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
        contentHeight: eventDetailData.height

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
            id: eventDetailData
            width: parent.width

            PageHeader {
                title: "Event details"
            }
            Label {
                text: conf_event != null ? conf_event.title : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeMedium
            }
            Label {
                text: conf_event != null ? conf_event.subtitle : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
                visible: conf_event != null && conf_event.subtitle.length > 0
            }
            Label {
                text: conf_event != null ? conf_event.room : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                text: conf_event != null ? conf_event.persons.toString() : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                text: conf_event != null ? Qt.formatDateTime(conf_event.starttime, "dddd yyyy-MM-dd HH:mm") + " - " + Qt.formatDateTime(conf_event.endtime, "HH:mm") : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
            }
            TextSwitch {
                text: "Favorite"
                enabled: conf_event != null
                visible: enabled
                automaticCheck: false
                checked: conf_event != null && conf_event.favorite
                onClicked: {
                    busy = true;
                    conf_event.favorite = !conf_event.favorite;
                    busy = false;
                }
            }

            SectionHeader {
                text: "Abstract"
                visible: lbl_abstract.visible
            }
            Label {
                id: lbl_abstract
                text: conf_event != null ? conf_event.abstract : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeTiny
                visible: conf_event != null && conf_event.abstract.length > 0
            }

            SectionHeader {
                text: "Description"
                visible: lbl_description.visible
            }
            Label {
                id: lbl_description
                text: conf_event != null ? conf_event.description : ""
                wrapMode: TextEdit.Wrap
                width: parent.width
                font.pixelSize: Theme.fontSizeTiny
                visible: conf_event != null && conf_event.description.length > 0
            }
        }
    }
}
