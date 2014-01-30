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
            conf.update(false, true);
        }
    }

    SilicaListView {
        id: confDetailView
        //contentHeight: confData.height

        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: "Update"
                onClicked: conf.update(true, true)
            }
            MenuItem {
                text: "Sort events"
                onClicked: confDetailView.model.sort(0)
                enabled: confDetailView.count > 0 && conf !== null
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
                text: conf != null ? conf.subtitle : ""
                visible: conf != null && conf.subtitle.length > 0
                font.pixelSize: Theme.fontSizeMedium
            }
            Label {
                text: conf != null ?
                          Qt.formatDate(conf.starttime, Qt.LocalDate) + " - " + Qt.formatDate(conf.endtime, Qt.LocalDate)
                        : ""
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                text: conf != null ?
                          (conf.venue.length > 0 ? conf.venue : "") +
                          (conf.venue.length > 0 && conf.city.length > 0 ? ", " : "") +
                          (conf.city.length > 0 ? conf.city : "")
                        : ""
                font.pixelSize: Theme.fontSizeSmall
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
            contentHeight: eventTitleLabel.height

            //Column {
                Label {
                    id: eventTitleLabel
                    text: model.title
                    wrapMode: TextEdit.Wrap
                    width: parent.width
                    font.pixelSize: Theme.fontSizeSmall
                    font.bold: model.favorite
                }
           /*     Label {
                    text: model.subtitle.length !== 0 ? model.subtitle : ""
                }

            }*/
           menu: ContextMenu {
               MenuItem {
                   text: model.favorite ? "Remove favorite" : "Add favorite"
                   onClicked: {
                       //confDetailView.model.get(model.event_id).favorite = !confDetailView.model.get(model.event_id).favorite;
                       confDetailView.model.make_item_favorite(index, !model.favorite);
                   }
               }
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
