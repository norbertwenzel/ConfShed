import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    property Conference conf: null

    function sort_events(criterion, text) {
        console.log("sort_events(" + (text ? text : criterion) + ")");
        confDetailView.model.sort_by(criterion);
        if(text)
        {
            sortMenuEntry.text = "Sort by: " + text;
        }
    }

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
            visible: (confDetailView.count > 0 || (confDetailView.model !== null && confDetailView.model.unfilteredLength > 0)) && conf !== null

            MenuItem {
                text: "Update"
                onClicked: conf.update(true, true)
            }

            MenuItem {
                text: "Filter for: None"
                onClicked: {
                    var dialog = pageStack.push(Qt.resolvedUrl("TextOptionDialogPage.qml"), { options : { "Tracks" : confDetailView.model.tracks,
                                                                                                          "Rooms" : confDetailView.model.rooms,
                                                                                                          "Days" : confDetailView.model.days },
                                                                                              comboLabel : "Filter for"});
                    dialog.accepted.connect(function() {
                        if(dialog.mainSelection === "Tracks") {
                            confDetailView.model.filter_by(ConferenceEventList.FilterTrack, dialog.subSelection);
                            sort_events(ConferenceEventList.SortTrack, dialog.mainSelection);
                        }
                        else if(dialog.mainSelection === "Rooms") {
                            confDetailView.model.filter_by(ConferenceEventList.FilterRoom, dialog.subSelection);
                            sort_events(ConferenceEventList.SortRoom, dialog.mainSelection);
                        }
                        else if(dialog.mainSelection === "Days") {
                            confDetailView.model.filter_by(ConferenceEventList.FilterDay, dialog.subSelection);
                            sort_events(ConferenceEventList.SortDay, dialog.mainSelection);
                        }
                        else {
                            console.error("Unknown selection type '" + dialog.mainSelection + "'.");
                            return; //we did not filter, so we do not need to enable clearing the subSelection
                        }
                        text = "Filter for: " + dialog.mainSelection;
                        clearFilterMenu.visible = true;
                    });
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            MenuItem {
                id: sortMenuEntry
                text: "Sort by: None"
                onClicked: {
                    var dialog = pageStack.push(Qt.resolvedUrl("TextOptionDialogPage.qml"), { options : [ "Titles", "Tracks", "Days", "Rooms" ],
                                                                                              comboLabel : "Sort by"});
                    dialog.accepted.connect(function() {
                        if(dialog.mainSelection === "Titles") {
                            sort_events(ConferenceEventList.SortTitle, dialog.mainSelection);
                        }
                        else if(dialog.mainSelection === "Tracks") {
                            sort_events(ConferenceEventList.SortTrack, dialog.mainSelection);
                        }
                        else if(dialog.mainSelection === "Rooms") {
                            sort_events(ConferenceEventList.SortRoom, dialog.mainSelection);
                        }
                        else if(dialog.mainSelection === "Days") {
                            sort_events(ConferenceEventList.SortDay, dialog.mainSelection);
                        }
                        else {
                            console.error("Unknown selection type '" + dialog.mainSelection + "'.");
                        }
                    });
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            MenuItem {
                text: "Upcoming events"
                onClicked: {
                    confDetailView.model.filter_by(ConferenceEventList.FilterCurrentTime);
                    clearFilterMenu.visible = true;
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            MenuItem {
                text: "Favorites"
                onClicked: {
                    confDetailView.model.filter_by(ConferenceEventList.FilterFavorite);
                    clearFilterMenu.visible = true;
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            MenuItem {
                id: clearFilterMenu
                text: "Clear filters"
                onClicked: {
                    confDetailView.model.filter_by(ConferenceEventList.FilterNone);
                    visible = false;
                }
                visible: false
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
