import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    property Conference conf: null

    SilicaListView {
        id: confEventList

        anchors.fill: parent
        //model: ConferenceList{}

        header: PageHeader {
            title: conf != null ? conf.title : ""
        }

        PullDownMenu {

        }

        ViewPlaceholder {
            enabled: confEventList.count == 0
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

        delegate: ListItem {

            contentHeight: column.height
            menu: ContextMenu {
            }

            Column {
                id: column
            }
        }
    }
}
