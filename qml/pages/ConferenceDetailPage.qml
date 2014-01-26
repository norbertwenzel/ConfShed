import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    property Conference conf: null

    function show_event_list() {
        console.log("show_event_list()");
        if(conf != null) {
            confDetailView.model = conf.events;
        }
    }

    Component.onCompleted: {
        if(conf != null) {
            conf.eventsChanged.connect(show_event_list);
            conf.update();
        }
    }

    SilicaListView {
        id: confDetailView
        //contentHeight: confData.height

        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: "Sort events"
                onClicked: conf.sort_events()
                enabled: conf !== null
            }
        }

        Connections {
            target: conf_sched
            onError: {
                console.error(message);
            }
        }

        ViewPlaceholder {
            enabled: confDetailView.count === 0
            text: "No events available"
            //hintText: "Pull down to add a conference"
        }

        VerticalScrollDecorator {}

        header: Column {
            width: parent.width

            PageHeader {
                title: conf != null ? conf.title : "Conference"
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

        section {
            property: 'track'
            delegate: SectionHeader {
                text: section
            }
        }

        delegate: ListItem {
            width: confDetailView.width
            contentHeight: confTitleLable.height

            //Column {
                Label {
                    id: confTitleLable
                    text: model.title
                    wrapMode: TextEdit.Wrap
                    width: parent.width
                    font.pixelSize: Theme.fontSizeSmall
                }
           /*     Label {
                    text: model.subtitle.length !== 0 ? model.subtitle : ""
                }

            }*/
           menu: ContextMenu {
               MenuItem {
                   text: "Show details"
                   onClicked: {
                       console.log(index + ": " + model.title)
                       pageStack.push(Qt.resolvedUrl("EventDetailPage.qml"), { conf_event: confDetailView.model.get(model.event_id) });
                   }
               }
           }
        }
    }
}
