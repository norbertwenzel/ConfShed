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

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        BusyIndicator {
            id: indicator

            anchors.centerIn: parent
            running: false
            size: BusyIndicatorSize.Large
        }

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            DialogHeader { //TODO accesses a null property when reading 'width'
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
                    var http = new XMLHttpRequest()
                    var url = text;

                    http.onreadystatechange = function() { // Call a function when the state changes.
                                if (http.readyState == 4) {
                                    indicator.running = false
                                    if (http.status == 200) {
                                        //console.log("ok")
                                        Qt.inputMethod.hide()
                                        page.canAccept = true
                                    } else {
                                        //console.log("some http error")
                                        errorHighlight = true //TODO show proper error message to user
                                    }
                                }
                            }
                    indicator.running = true
                    http.open("HEAD", url, true);
                    http.send();
                }
            }
        }
    }
}
