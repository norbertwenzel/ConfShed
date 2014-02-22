import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    property Conference conf: null

    function sort_events(criterion) {
        console.log("sort_events(" + criterion + ")");
        confDetailView.model.sort_by(criterion);
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
            visible: confDetailView.count > 0 && conf !== null

            MenuItem {
                text: "Update"
                onClicked: conf.update(true, true)
            }
            MenuItem {
                text: "Sort by: day"
                /*onClicked: {
                    confDetailView.model.sort_by(ConferenceEventList.SortDay);
                    confDetailView.section.property = 'weekday';
                    confDetailView.section.delegate = {
                        text: confDetailView.section
                    }
                }*/
                onClicked: {
                    var dialog = pageStack.push(Qt.resolvedUrl("TextOptionDialogPage.qml"), { options : { "Tracks" : confDetailView.model.tracks,
                                                                                                          "Rooms" : confDetailView.model.rooms }});
                    dialog.accepted.connect(function() {
                        if(dialog.filterType === "Tracks") {
                            confDetailView.model.filter_by(ConferenceEventList.FilterTrack, dialog.filter);
                            sort_events(ConferenceEventList.SortTrack);
                        }
                        else if(dialog.filterType === "Rooms") {
                            confDetailView.model.filter_by(ConferenceEventList.FilterRoom, dialog.filter);
                            sort_events(ConferenceEventList.SortRoom);
                        }
                        else {
                            console.error("Unknown filter type '" + dialog.filterType + "'.");
                            return; //we did not filter, so we do not need to enable clearing the filter
                        }
                        clearFilterMenu.enabled = true;
                    });
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            /*MenuItem {
                text: "Sort by title"
                onClicked: {
                    confDetailView.model.sort_by(ConferenceEventList.SortTitle)
                    confDetailView.section.property = 'title';
                    confDetailView.section.criteria = ViewSection.FirstCharacter;
                    confDetailView.section.delegate = {
                        text: confDetailView.section.slice(0, 1)
                    }
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            MenuItem {
                text: "Sort by track"
                onClicked: {
                    confDetailView.model.sort_by(ConferenceEventList.SortTrack)
                    confDetailView.section.property = 'track'
                    confDetailView.section.criteria = ViewSection.FullString
                    confDetailView.section.delegate = {
                        text: confDetailView.section
                    }
                }
                enabled: confDetailView.count > 0 && conf !== null
            }*/
            MenuItem {
                text: "Filter for: None"
                onClicked: pageStack.push(Qt.resolvedUrl("TextOptionDialogPage.qml"), {  });
            }
            MenuItem {
                text: "Upcoming events"
                onClicked: {
                    confDetailView.model.filter_by(ConferenceEventList.FilterCurrentTime, "");
                }
            }
            MenuItem {
                id: clearFilterMenu
                text: "Clear filters"
                onClicked: {
                    confDetailView.model.filter_by(ConferenceEventList.FilterNone, "");
                    enabled = false;
                }
                enabled: false
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

            Column {
                width: parent.width

                Row {
                    width: parent.width

                    //TODO: export in special qml element
                    ComboBox {
                        id: filterSelection
                        width: parent.width / 2
                        label: "Filter: "
                        currentIndex: 0

                        menu: null

                        onCurrentIndexChanged: {
                            applyFilterBtn.enabled = true;
                            filterParamDay.enabled = false;
                            filterParamTrack.enabled = false;

                            if(currentIndex == 1)
                            {
                                filterParamTrack.enabled = true;
                            }
                            else if(currentIndex == 2)
                            {
                                filterParamDay.enabled = true;
                            }
                        }
                    }
                    ComboBox {
                        id: filterParamDay
                        width: parent.width / 2
                        label: "for"
                        currentIndex: 0
                        enabled: filterSelection.currentIndex === 2
                        visible: enabled

                        menu: null

                        onCurrentIndexChanged: applyFilterBtn.enabled = true
                    }
                    ComboBox {
                        id: filterParamTrack
                        width: parent.width / 2
                        label: "for"
                        currentIndex: 0
                        enabled: filterSelection.currentIndex === 1
                        visible: enabled

                        menu: null

                        onCurrentIndexChanged: applyFilterBtn.enabled = true
                    }
                }

                Button {
                    id: applyFilterBtn
                    text: "Apply filter"
                    enabled: false

                    onClicked: {
                        enabled = false;
                        if(filterSelection.currentIndex == 0)
                        {
                            confDetailView.model.filter_by(ConferenceEventList.FilterNone, "");
                        }
                        else if(filterSelection.currentIndex == 1)
                        {
                            confDetailView.model.filter_by(ConferenceEventList.FilterTrack, filterParamTrack.menu._contentColumn.children[filterParamTrack.currentIndex].text);
                        }
                        else if(filterSelection.currentIndex == 2)
                        {
                            confDetailView.model.filter_by(ConferenceEventList.FilterDay, filterParamDay.menu._contentColumn.children[filterParamDay.currentIndex].text);
                        }
                        else if(filterSelection.currentIndex == 4)
                        {
                            confDetailView.model.filter_by(ConferenceEventList.FilterCurrentTime, "");
                        }
                    }
                }
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
