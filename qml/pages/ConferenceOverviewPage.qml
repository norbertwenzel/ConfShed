import QtQuick 2.0
import Sailfish.Silica 1.0
import ConfSched 0.1

Page {
    id: page

    function add_conference_dialog(error_url) {
        var dialog = pageStack.push(Qt.resolvedUrl("AddConferencePage.qml"));
        /*dialog.onNewConfDataUrlChanged.connect(function() { console.log("the value changed to " + dialog.newConfDataUrl) });
        dialog.accepted.connect(
            function() {
                console.log("The url set by the user: " + dialog.newConfDataUrl);
                conf_sched.addConference(dialog.newConfDataUrl)
            });*/
    }

    function set_conference_list(conf_list) {
        confOverviewList.model = conf_list;
    }

    function add_debug_conferences() {
        conf_sched.addConference("https://fosdem.org/2014/schedule/xml");
        conf_sched.addConference("https://cfp.linuxwochen.at/de/lww2013/public/schedule.xml");
        conf_sched.addConference("http://sigint.ccc.de/schedule/schedule.xml");
        conf_sched.addConference("https://pentabarf.junge-piraten.de/fahrplan/om13/schedule.de.xml");
        conf_sched.addConference("https://pentabarf.junge-piraten.de/fahrplan/om12/schedule.de.xml");
    }

    Component.onCompleted: {
        var confs = conf_sched.get_all_conferences();
        if(confs.length === 1)
        {
            console.log("only one conf available");
        }
        else
        {
            console.log("there are " + confs.length + " conferences waiting for you");
        }
        set_conference_list(confs);
    }

    SilicaListView {
        id: confOverviewList

        anchors.fill: parent
        model: ConferenceList{}

        header: PageHeader {
            title: "Conferences"
        }

        PullDownMenu {
            MenuItem {
                text: "Add debug data"
                onClicked: add_debug_conferences();
                visible: conf_sched.DEBUG
            }
            MenuItem {
                text: "Add conference"
                onClicked: add_conference_dialog();
            }
            MenuItem {
                text: "Update all conferences"
                onClicked: conf_sched.updateAllConferences();
                visible: confOverviewList.model.length > 0
            }
        }

        ViewPlaceholder {
            enabled: confOverviewList.count == 0
            text: "No conferences configured"
            hintText: "Pull down to add a conference"
        }

        Connections {
            target: conf_sched
            onConferenceAdded: {
                console.log("Conference added: " + conf);
            }
            onConferenceListChanged: {
                console.log("Conference list changed.");
                set_conference_list(confs);
            }
            onError: {
                console.error(message);
            }
        }

        VerticalScrollDecorator {}

        delegate: ListItem {

            function remove() {
                remorseAction("Deleting " + model.title, function() { confOverviewList.model.removeRow(index); });
            }
            function show_details(item) {
                console.log("show_details[" + index + "]: " + item.conf_id + " " + item.title);
                pageStack.push(Qt.resolvedUrl("ConferenceDetailPage.qml"), { conf: item });
            }

            contentHeight: column.height
            menu: ContextMenu {
                MenuItem {
                    text: "Show events"
                    onClicked: show_details(confOverviewList.model.get(model.conf_id))
                }
                MenuItem {
                    text: "Delete"
                    onClicked: remove();
                }
            }

            Column {
                id: column

                Label {
                    text: model.title
                }
                Label {
                    text: model.subtitle
                }
                Label {
                    text: model.venue
                }
                Label {
                    text: model.city
                }
            }
        }
    }
}


