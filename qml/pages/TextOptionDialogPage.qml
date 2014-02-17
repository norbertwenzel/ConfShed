import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: page

    property var options: ({})
    property string filterType: ""
    property string filter: ""

    property int __defaultIndex: 0
    property var __menus: ({})
    property int __selectedCnt: 0

    function init_submenu_model() {
        var selected_key = Object.keys(options);

        if(selected_key.length > 0)
        {
            textOptionView.model = __menus[selected_key[__defaultIndex]];
            filterType = selected_key[__defaultIndex];
        }
        else
        {
            console.log("Menu has no options.");
        }
    }

    function set_submenu_model(key) {
        if(key in __menus)
        {
            var menu = __menus[key];
            clear_menu_selection(menu);
            textOptionView.model = menu;
            filterType = key;
        }
        else
        {
            console.log("Invalid menu option " + key);
        }
    }

    function clear_menu_selection(model) {
        for(var i = 0; i < model.count; i++) {
            model.setProperty(i, "selected", false);
        }
        __selectedCnt = 0;
    }

    Component.onCompleted: {
        for(var opt in options) {
            console.log(opt + ":");
            console.log(options[opt].length, options[opt]);

            //create a list model for every item
            __menus[opt] = Qt.createQmlObject('import QtQuick 2.0; import Sailfish.Silica 1.0; ListModel { }',
                                              page, opt + "ListModel");
            //fill the list model
            options[opt].forEach(function(entry) {
                __menus[opt].append({"option": entry, "selected": false});
            });
        }

        init_submenu_model();
    }

    canAccept: __selectedCnt > 0
    onDone: {
        if(result === DialogResult.Accepted)
        {
            for(var i = 0; i < textOptionView.model.count; i++) {
                if(textOptionView.model.get(i).selected)
                {
                    if(filter.length > 0) {
                        filter += "|";
                    }

                    filter += textOptionView.model.get(i).option;
                }
            }

            console.log("Filtering '" + filterType + "': " + filter);
        }
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

            width: parent.width
            highlighted: model.selected
            onClicked: {
                textOptionView.model.setProperty(index, "selected", !model.selected);
                highlighted = model.selected;
                if(model.selected) __selectedCnt++;
                else __selectedCnt--;
            }
        }
    }
}
