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

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: "Update conferences"
                onClicked: conf_sched.updateAllConferences();
                enabled: confOverviewList.model.length > 0
            }
            MenuItem {
                text: "Add conference"
                onClicked: add_conference_dialog();
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

            function remove(item) {
                remorseAction("Deleting " + item.title, function() { conf_sched.removeConference(item); });
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
                    onClicked: remove(confOverviewList.model.get(model.conf_id))
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


