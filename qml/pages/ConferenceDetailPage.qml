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
                text: "Sort by day"
                onClicked: {
                    confDetailView.model.sort_by(ConferenceEventList.SortDay);
                    confDetailView.section.property = 'weekday';
                    confDetailView.section.delegate = {
                        text: confDetailView.section
                    }
                }
                enabled: confDetailView.count > 0 && conf !== null
            }
            MenuItem {
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

                        menu: ContextMenu {
                            MenuItem { text: "None" }
                            MenuItem { text: "Track" }
                            MenuItem { text: "Day" }
                            MenuItem { text: "Upcoming" }
                        }

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

                        menu: ContextMenu{
                            MenuItem { text: "Saturday" }
                            MenuItem { text: "Sunday" }
                        }

                        onCurrentIndexChanged: applyFilterBtn.enabled = true
                    }
                    ComboBox {
                        id: filterParamTrack
                        width: parent.width / 2
                        label: "for"
                        currentIndex: 0
                        enabled: filterSelection.currentIndex === 1
                        visible: enabled

                        menu: ContextMenu{
                            MenuItem { text: "Ada" }
                            MenuItem { text: "Automotive development" }
                            MenuItem { text: "BSD" }
                            MenuItem { text: "Certification" }
                            MenuItem { text: "Configuration management" }
                            MenuItem { text: "Desktops" }
                            MenuItem { text: "Distributions" }
                            MenuItem { text: "Embedded" }
                            MenuItem { text: "Energy-efficient computing" }
                            MenuItem { text: "Game development" }
                            MenuItem { text: "Go" }
                            MenuItem { text: "Graph processing" }
                            MenuItem { text: "Graphics" }
                            MenuItem { text: "HPC and computational science" }
                            MenuItem { text: "Hardware" }
                            MenuItem { text: "IPv6" }
                            MenuItem { text: "Internet of things" }
                            MenuItem { text: "Java" }
                            MenuItem { text: "JavaScript" }
                            MenuItem { text: "Keynotes" }
                            MenuItem { text: "LLVM" }
                            MenuItem { text: "Legal and policy issues" }
                            MenuItem { text: "Lightning talks" }
                            MenuItem { text: "Mail" }
                            MenuItem { text: "Mathematics" }
                            MenuItem { text: "Memory and storage" }
                            MenuItem { text: "Microkernel-based operating systems" }
                            MenuItem { text: "Miscellaneous" }
                            MenuItem { text: "Mozilla" }
                            MenuItem { text: "MySQL" }
                            MenuItem { text: "NoSQL" }
                            MenuItem { text: "Open document editors" }
                            MenuItem { text: "Perl" }
                            MenuItem { text: "PostgreSQL" }
                            MenuItem { text: "Python" }
                            MenuItem { text: "Security" }
                            MenuItem { text: "Smalltalk" }
                            MenuItem { text: "Software defined radio" }
                            MenuItem { text: "Testing and automation" }
                            MenuItem { text: "Tracing and debugging" }
                            MenuItem { text: "Valgrind" }
                            MenuItem { text: "Virtualisation and IaaS" }
                            MenuItem { text: "Wikis" }
                            MenuItem { text: "Wine" }
                        }

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
