/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

    // To enable PullDownMenu, place our content in a SilicaFlickable
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
                //pageStack.push(Qt.resolvedUrl("event_details_page.qml"), { conf_event: eventModel.get_event(item.id) })
            }

            contentHeight: column.height
            menu: ContextMenu {
                MenuItem {
                    text: "Delete"
                    onClicked: remove(confOverviewList.model.get(model.conf_id))
                }
                MenuItem {
                    text: "Show events"
                    onClicked: show_details(confOverviewList.model.get(model.conf_id))
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


