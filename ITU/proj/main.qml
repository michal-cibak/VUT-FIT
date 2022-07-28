import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("basicTODOlist")

    property bool rem_note: true //reminder or note mode active
    property variant themes: [
        Qt.lighter('gold'), //app background for REMINDER, border is 1.2 times lighter
        'skyblue', //app background for NOTE, border is 1.2 times lighter
        'gold', //"Nová pripomienka" button filling (REMINDER)
        Qt.lighter('royalblue', 1.2)] //"Nová poznámka" button filling (NOTE)
    property color theme: rem_note ? themes[0] : themes[1] //used theme
    property int rempagestotal: reminders.count !== 0 ? (reminders.count - 1) / 16 + 1 : 1 //on how many pages are reminders
    property int notepagestotal: notes.count !== 0 ? (notes.count - 1) / 16 + 1 : 1 //on how many pages are notes
    property int rempageactual: 1 //which page of reminders are you (/ were last) on
    property int notepageactual: 1 //which page of notes are you (/ were last) on
    property int indexofclicked: -1 //which reminder / note has been clicked on
    property bool editing: false //editing existing reminder or note (not adding new one) - true after clicking on existing one
    property int space: (menu.width - 128) / 2 //space around and between buttons in menus (128 == Button.width)
    property bool date_importance: true //sort reminders by date or importance
    property date currentdate: new Date(new Date().getFullYear(), new Date().getMonth(), new Date().getDate()) //static date, "actualdate()" would be called here instead, but javascript's timer doesn't work :/

    function refresh() {
        var temp = rempageactual
        rempageactual = rempagestotal + 1
        rempageactual = temp
    }

    function startediting() {
        menu_main.visible = false
        menu_edit.visible = true
        edittab.visible = true
        fridge.visible = false //necessary, reminders/notes stay clickable even when covered
    }

    function endediting() {
        menu_edit.visible = false
        menu_main.visible = true
        edittab.visible = false
        fridge.visible = true
    }

    /*
    function actualdate() { //sets current date and schedules daily re-evaluation one second after midnight
        var datenow = new Date()
        currentdate = new Date(datenow.getFullYear(), datenow.getMonth(), datenow.getDate())
        setTimeout(function(){ setInterval(setcurrentdate(), 86400000) }, 86400000 - datenow.getHours() * 3600000 - datenow.getMinutes() * 60000 - datenow.getSeconds() * 1000 - datenow.getMilliseconds() + 1000)
    }

    function setcurrentdate() {
        var datenow = new Date()
        currentdate = new Date(datenow.getFullYear(), datenow.getMonth(), datenow.getDate())
    }
    */

    ListModel {
        id: reminders;
        //ListElement { date: "yyyy-MM-dd"; text: "I am a reminder!"; importance: 0 } //date is actually of type date
    }
    ListModel {
        id: notes;
        //ListElement { text: "I am a note!" }
    }

    Row {
        //MENUS
        Frame {
            id: menu
            width: root.width / 4
            height: root.height
            color: theme

            //MAIN MENU
            Item {
                id: menu_main
                width: parent.width
                height: parent.height

                //ADD REMINDER/NOTE BUTTONS
                Column {
                    id: addbutts
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: space
                    spacing: space

                    Button {
                        text: "Nová pripomienka"
                        color: themes[2]
                        onClick: {
                            rem_note = true
                            editrem.importance = 0
                            editrem.date = "DD.MM.RRRR"
                            editrem.datecolor = 'black'
                            editrem.text = ""
                            editrem.notefocus = true
                            startediting()
                        }
                    }

                    Button {
                        text: "Nová poznámka"
                        color: themes[3]
                        onClick: {
                            rem_note = false
                            editnote.text = ""
                            editnote.focus = true
                            startediting()
                        }
                    }
                }
                //~!ADD REMINDER/NOTE BUTTONS

                //SORTING METHOD BUTTONS
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: addbutts.bottom
                    anchors.topMargin: 2 * space + 32 //32 == Button.height
                    spacing: space
                    visible: rem_note ? true : false

                    Text {
                        text: "Radiť podľa:"
                    }

                    Button {
                        text: "Dátumu"
                        opacity: date_importance ? 0.75 : 1
                        onClick: {
                            if (!date_importance) {
                                sortbydate()
                                date_importance = true
                                refresh()
                            }
                        }

                        /*
                        Rectangle { //extra border for better visibility in case someone wants it
                            width: parent.width
                            height: parent.height
                            color: Qt.rgba(0, 0, 0, 0)
                            border.width: 2
                            border.color: 'blue'
                            visible: date_importance ? true : false
                            opacity: 0.25
                        }
                        */

                        function sortbydate() {
                            if (reminders.count > 1) {
                                var index, impstartex = [-1, -1, -1], impcount = [0, 0, 0]
                                for (index = 0; index < reminders.count; index++)
                                    ++impcount[reminders.get(index).importance]
                                if (impcount[2])
                                    impstartex[2] = 0
                                if (impcount[1])
                                    impstartex[1] = impcount[2]
                                if (impcount[0])
                                    impstartex[0] = reminders.count - impcount[0]
                                index = 0
                                for (var min; index < reminders.count; index++) {
                                    if (impcount[2]) {
                                        min = 2
                                        if (impcount[1] && reminders.get(impstartex[1]).date.getTime() < reminders.get(impstartex[2]).date.getTime())
                                            min = 1
                                        if (impcount[0] && reminders.get(impstartex[0]).date.getTime() < reminders.get(impstartex[min]).date.getTime())
                                            min = 0
                                    }
                                    else if (impcount[1]) {
                                        min = 1
                                        if (impcount[0] && reminders.get(impstartex[0]).date.getTime() < reminders.get(impstartex[1]).date.getTime())
                                            min = 0
                                    }
                                    else
                                        min = 0
                                    reminders.move(impstartex[min], reminders.count - 1, 1)
                                    if (min === 2) {
                                        --impstartex[1]
                                        --impstartex[0]
                                    }
                                    else if (min === 1)
                                        --impstartex[0]
                                    --impcount[min]
                                }
                            }
                        }
                    }

                    Button {
                        text: "Dôležitosti a dátumu"
                        opacity: date_importance ? 1 : 0.75
                        onClick: {
                            if (date_importance) {
                                sortbyimp()
                                date_importance = false
                                refresh()
                            }
                        }

                        /*
                        Rectangle { //extra border for better visibility in case someone wants it
                            width: parent.width
                            height: parent.height
                            color: Qt.rgba(0, 0, 0, 0)
                            border.width: 2
                            border.color: 'blue'
                            visible: date_importance ? false : true
                            opacity: 0.25
                        }
                        */

                        function sortbyimp() {
                            if (reminders.count > 1) {
                                var index = 1, impendex = [0, 0, 0], impcount = [0, 0, 0]
                                impcount[reminders.get(0).importance] = 1
                                while (index < reminders.count) {
                                    if (!impcount[reminders.get(index).importance]) {
                                        if (impcount[2]) {
                                            reminders.move(index - impcount[2], impendex[2], impcount[2])
                                            impendex[2] += impcount[2]
                                            impendex[1] += impcount[2]
                                            impendex[0] += impcount[2]
                                            impcount[2] = 0
                                        }
                                        else if (impcount[1]) {
                                            reminders.move(index - impcount[1], impendex[1], impcount[1])
                                            impendex[1] += impcount[1]
                                            impendex[0] += impcount[1]
                                            impcount[1] = 0
                                        }
                                        else if (impcount[0]) {
                                            reminders.move(index - impcount[0], impendex[0], impcount[0])
                                            impendex[0] += impcount[0]
                                            impcount[0] = 0
                                        }
                                    }
                                    ++impcount[reminders.get(index).importance]
                                    ++index
                                }
                                if (impcount[2]) {
                                    reminders.move(index - impcount[2], impendex[2], impcount[2])
                                    impendex[2] += impcount[2]
                                    impendex[1] += impcount[2]
                                    impendex[0] += impcount[2]
                                    impcount[2] = 0
                                }
                                else if (impcount[1]) {
                                    reminders.move(index - impcount[1], impendex[1], impcount[1])
                                    impendex[1] += impcount[1]
                                    impendex[0] += impcount[1]
                                    impcount[1] = 0
                                }
                                else {
                                    reminders.move(index - impcount[0], impendex[0], impcount[0])
                                    impendex[0] += impcount[0]
                                    impcount[0] = 0
                                }
                            }
                        }
                    }
                }
                //~!SORTING METHOD BUTTONS

                //EXIT BUTTON (formerly for testing, kept because of positive feedback)
                Button {
                    text: "Skončiť"
                    color: 'red'
                    radius: 16
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: space
                    onClick: {
                        Qt.quit();
                    }
                }
                //~!EXIT BUTTON
            }
            //~!MAIN MENU

            //EDIT MENU
            Column {
                id: menu_edit
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: space
                spacing: space
                visible: false

                Button {
                    text: rem_note ? "Uložiť pripomienku" : "Uložiť poznámku"
                    color: 'greenyellow'
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClick: {
                        if (rem_note) {
                            var isodate = new Date(0)
                            mydatetoisodate()
                            if (isdate()) {
                                if (!editing)
                                    reminders.insert(indextoinsertto(), {"date": isodate, "text": editrem.text, "importance": editrem.importance})
                                else {
                                    var indextomoveto = indextoinsertto()
                                    reminders.set(indexofclicked, {"date": isodate, "text": editrem.text, "importance": editrem.importance})
                                    reminders.move(indexofclicked, indextomoveto, 1)
                                    refresh()
                                    editing = false
                                }
                                endediting()
                            }
                            else {
                                editrem.datecolor = 'red'
                                editrem.date = "DD.MM.RRRR"
                                focus = true
                            }

                            function mydatetoisodate() {
                                var parts = editrem.date.split('.')
                                isodate = new Date(parts[2], parts[1] - 1, parts[0])
                            }

                            function isdate() {
                                //even though example shows a date should look like "DD.MM.YYYY", the rules aren't that strict
                                //as long as there are 2 dots and no illegal characters (spaces are ok), it will complete itself to a valid date
                                if (isodate instanceof Date && !isNaN(isodate)) {
                                    var dots = 0
                                    for (var i = 0; i < editrem.datelength; i++)
                                        if (editrem.date[i] === ".")
                                            ++dots
                                    if (dots === 2)
                                        return true
                                }
                                return false
                            }

                            function indextoinsertto() {
                                var index = 0
                                if (date_importance)
                                    while (index < reminders.count && (isodate.getTime() > reminders.get(index).date.getTime() || (isodate.getTime() === reminders.get(index).date.getTime() && editrem.importance < reminders.get(index).importance)))
                                        ++index
                                else
                                    while (index < reminders.count && (editrem.importance < reminders.get(index).importance || (editrem.importance === reminders.get(index).importance && isodate.getTime() > reminders.get(index).date.getTime())))
                                        ++index
                                if (editing && index > indexofclicked)
                                    --index
                                return index
                            }
                        }
                        else {
                            if (!editing)
                                notes.insert(0, {"text": editnote.text})
                            else {
                                notes.set(indexofclicked, {"text": editnote.text})
                                editing = false
                            }
                            endediting()
                        }
                    }
                    Keys.onTabPressed:
                        editrem.datefocus = true
                }

                Button {
                    text: "Zmeniť dôležitosť"
                    color: 'yellow'
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: rem_note ? true : false
                    onClick: {
                        editrem.importance = ((editrem.importance) + 1) % 3
                    }
                }

                Button {
                    text: editing ? "Zahodiť zmeny" : (rem_note ? "Zahodiť pripomienku" : "Zahodiť poznámku")
                    color: 'orange'
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClick: {
                        editing = false //no need for: if (editing)
                        endediting()
                    }
                }

                Button {
                    text: rem_note ? "Zmazať pripomienku" : "Zmazať poznámku"
                    color: Qt.lighter('red', 1.2)
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: editing ? true : false
                    onClick: {
                        if (rem_note)
                            reminders.remove(indexofclicked)
                        else
                            notes.remove(indexofclicked)
                        editing = false //no need for: if (editing)
                        endediting()
                    }
                }
            }
            //~!EDIT MENU
        }
        //~!MENUS

        //SCENE (workspace)
        Item {
            id: scene
            width: root.width - menu.width
            height: root.height

            //FRIDGE
            Column {
                id: fridge
                //TYPE
                Row {
                    Button {
                        id: buttrem
                        width: scene.width / 2
                        color: themes[0]
                        border.color: Qt.lighter(theme, 1.2)
                        border.width: 5
                        text: "Pripomienky"
                        onClick: {
                            rem_note = true
                        }
                    }

                    Button {
                        width: buttrem.width
                        color: themes[1]
                        border.color: Qt.lighter(theme, 1.2)
                        border.width: 5
                        text: "Poznámky"
                        onClick: {
                            rem_note = false
                        }
                    }
                }
                //~!TYPE

                //BODY
                Frame {
                    width: scene.width
                    height: scene.height-buttrem.height
                    color: theme

                    //REMINDER GRID
                    Item {
                        id: remindergrid
                        width: parent.width - 5 * 2 //- left and right border
                        height: parent.height - pages.height - 5 //- top border
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 5 //top border
                        visible: rem_note ? true : false

                        Grid {
                            anchors.centerIn: parent
                            spacing: 8
                            Repeater {
                                model: 16
                                Reminder {
                                    property int actindex: (rempageactual - 1) * 16 + index
                                    date: actindex < reminders.count ? reminders.get(actindex).date.toLocaleDateString(Qt.locale(), "dd. MM. yyyy") : ""
                                    datecolor: actindex < reminders.count && reminders.get(actindex).date.getTime() < currentdate.getTime() ? 'red' : 'black'
                                    text: actindex < reminders.count ? reminders.get(actindex).text : ""
                                    importance: actindex < reminders.count ? reminders.get(actindex).importance : 0
                                    visibility: actindex < reminders.count ? true : false
                                    onClick: {
                                        indexofclicked = actindex
                                        editing = true
                                        editrem.importance = reminders.get(indexofclicked).importance
                                        editrem.date = reminders.get(indexofclicked).date.toLocaleDateString(Qt.locale(), "dd.MM.yyyy")
                                        editrem.datecolor = 'black'
                                        editrem.text = reminders.get(indexofclicked).text
                                        editrem.cpos = editrem.length
                                        editrem.notefocus = true
                                        startediting()
                                    }
                                }
                            }
                        }
                    }
                    //~!REMINDER GRID

                    //NOTE GRID
                    Item {
                        id: notegrid
                        width: parent.width - 5 * 2 //- left and right border
                        height: parent.height - pages.height - 5 //- top border
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 5 //top border
                        visible: rem_note ? false : true

                        Grid {
                            anchors.centerIn: parent
                            spacing: 8
                            Repeater {
                                model: 16
                                Note {
                                    property int actindex: (notepageactual - 1) * 16 + index
                                    text: actindex < notes.count ? notes.get(actindex).text : ""
                                    visibility: actindex < notes.count ? true : false
                                    onClick: {
                                        indexofclicked = actindex
                                        editing = true
                                        editnote.text = notes.get(indexofclicked).text
                                        editnote.cpos = editnote.length
                                        editnote.focus = true
                                        startediting()
                                    }
                                }
                            }
                        }
                    }
                    //~!NOTE GRID=

                    //PAGE COUNTER
                    Pages {
                        id: pages
                        width: scene.width //no need to consider borders
                        height: 42
                        anchors.bottom: parent.bottom
                        text: rem_note ? (rempageactual + "/" + rempagestotal) : (notepageactual + "/" + notepagestotal)
                        leftbuttop: (rem_note && (rempageactual <= 1)) || (!rem_note && (notepageactual <= 1)) ? 0.25 : 1
                        rightbuttop: (rem_note && (rempageactual >= rempagestotal)) || (!rem_note && (notepageactual >= notepagestotal)) ? 0.25 : 1
                        onPrev: {
                            if (leftbuttop == 1)
                                if (rem_note)
                                    --rempageactual
                                else
                                    --notepageactual
                        }
                        onNext: {
                            if (rightbuttop == 1)
                                if (rem_note)
                                    ++rempageactual
                                else
                                    ++notepageactual
                        }
                    }
                    //~!PAGE COUNTER
                }
                //~!BODY
            }
            //~!FRIDGE

            //EDIT TAB (window for editing reminders/notes, replaces "FRIDGE")
            Frame {
                id: edittab
                width: parent.width
                height: parent.height
                color: theme
                visible: false

                Inputreminder {
                    id: editrem
                    anchors.centerIn: parent
                }

                Inputnote {
                    id: editnote
                    anchors.centerIn: parent
                    visible: rem_note ? false : true
                }
            }
            //~!EDIT TAB
        }
        //~!SCENE
    }
}
