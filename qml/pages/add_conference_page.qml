import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: page

    canAccept: false
    onAccepted: conf_sched.addConference(url_input.text)

    SilicaFlickable {
        anchors.fill: parent

        Connections {
            target: conf_sched
            onConferenceAdded: console.log("Conference added: " + conf_data_url)
        }

        BusyIndicator {
            id: indicator

            anchors.centerIn: parent
            running: false
            size: BusyIndicatorSize.Large
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width //TODO accesses a null property
            spacing: Theme.paddingLarge
            DialogHeader {
                acceptText: "Add new event"
                dialog: page
            }
            TextField {
                id: url_input
                placeholderText: "Enter Pentabarf URL"
                width: parent.width
                focus: true
                //text: "https://fosdem.org/2014/schedule/xml"
                inputMethodHints: Qt.ImhUrlCharactersOnly

                onTextChanged: { errorHighlight = false }

                EnterKey.enabled: text.length > 0
                EnterKey.onClicked: {
                    if(text.length > 0)
                    {
                        var http = new XMLHttpRequest()
                        var url = text;

                        http.onreadystatechange = function() { // Call a function when the state changes.
                                    if (http.readyState == 4) {
                                        indicator.running = false
                                        if (http.status == 200) {
                                            console.log("ok")
                                            Qt.inputMethod.hide()
                                            page.canAccept = true
                                        } else {
                                            console.log("some http error")
                                            errorHighlight = true
                                        }
                                    }
                                }
                        indicator.running = true
                        http.open("HEAD", url, true);
                        http.send();
                    }
                    else {
                        console.log("Text is too short: " + text.length)
                    }
                }
            }
        }
    }
}
