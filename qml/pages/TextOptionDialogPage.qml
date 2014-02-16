import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: page

    property var options: ({})

    property int __defaultIndex : 0
    property var __menus: ({})

    function init_submenu_model() {
        var selected_key = Object.keys(options);

        if(selected_key.length > 0)
        {
            textOptionView.model = __menus[selected_key[__defaultIndex]];
        }
        else
        {
            console.log("Menu has no options.");
        }
    }

    function set_submenu_model(key) {
        if(key in __menus)
        {
            textOptionView.model = __menus[key];
        }
        else
        {
            console.log("Invalid menu option " + key);
        }
    }

    Component.onCompleted: {
        for(var opt in options) {
            console.log(opt + ":");
            console.log(options[opt].length, options[opt]);

            //create a list model for every item
            __menus[opt] = Qt.createQmlObject('import QtQuick 2.0; import Sailfish.Silica 1.0; ListModel {}',
                                              page, opt + "ListModel");
            //fill the list model
            options[opt].forEach(function(entry) {
                __menus[opt].append({"option": entry});
            });
        }

        init_submenu_model();
    }

    canAccept: textOptionView.count > 0
    onAccepted: {
    }

    SilicaListView {
        id: textOptionView
        anchors.fill: parent

        VerticalScrollDecorator {}

        ViewPlaceholder {
            enabled: conf_sched.DEBUG && textOptionView.count === 0
            text: "Sorry!"
            hintText: "This is a bug. :-("
        }

        header:  Column {
            width: parent.width

            DialogHeader {
                acceptText: "Accept"
                dialog: page
            }
            ComboBox {
                id: mainMenu
                width: parent.width
                label: "Filter by"
                currentIndex: __defaultIndex
                onCurrentIndexChanged: set_submenu_model(value);

                menu: ContextMenu {
                    Repeater {
                        id: mainMenuRepeater
                        //use the array of keys as main menu options
                        model: options != null ? Object.keys(options)  : null;
                        MenuItem { text: modelData }
                    }
                }
            }
        }

        section {
            property: 'option'
            criteria: ViewSection.FirstCharacter
            delegate: SectionHeader {
                text: section.slice(0, 1)
            }
        }

        delegate: BackgroundItem {
            Label {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: model.option
            }
        }
    }
}
