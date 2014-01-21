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

    Component.onCompleted: {
        var confs = conf_sched.get_all_conferences();
        if(confs.length === 1)
        {
            text_pane.text = "only one conf available"
        }
        else
        {
            text_pane.text = "there are " + confs.length + " conferences waiting for you"
        }
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: "Add conference"
                onClicked: add_conference_dialog();
            }
        }

        Connections {
            target: conf_sched
            onConferenceAdded: {
                console.log("Conference added: " + conf);
                text_pane.text = conf.title + " in " + conf.city
            }
            onError: {
                console.error(message);
                text_pane.text = message
                text_pane.color = "red";
            }
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                id: page_header
                title: "Conferences"
            }
            Label {
                id: text_pane
                x: Theme.paddingLarge
                text: "Hello Sailors"
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeSmall

                /*Connections {
                    target: conf_sched
                    onConferenceAdded: text_pane.text = title;
                    onError: {
                        text_pane.text = message
                        text_pane.color = "red"
                    }
                }*/
            }
        }
    }
}


