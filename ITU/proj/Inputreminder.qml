import QtQuick 2.9

Item {
    //double size of regular
    id: root
    width: 208
    height: 180

    property int importance: 0
    property alias date: date_label.text
    property alias datelength: date_label.length
    property alias datecolor: date_label.color
    property alias datefocus: date_label.focus
    property alias text: note.text
    property alias length: note.length
    property alias cpos: note.cpos
    property alias notefocus: note.focus

    Column {
        id: reminder
        property variant colors: ['greenyellow', 'yellow', 'orange']

        Rectangle {
            id: date
            width: root.width
            height: 32
            color: reminder.colors[importance]
            border.width: 4
            border.color: Qt.darker(color)

            TextInput {
                id: date_label
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pixelSize: 26
                maximumLength: 10
                text: "DD.MM.RRRR"
                onFocusChanged: {
                    if (focus && text === "DD.MM.RRRR") {
                        text = ""
                        color = 'black'
                    }
                    else if (!focus && !length)
                        text = "DD.MM.RRRR"
                }
                KeyNavigation.tab: note
            }
        }

        Inputnote {
            id: note
            width: root.width
            height: root.height - date.height
            color: reminder.colors[importance]

            onTabpress: {
                date_label.focus = true
                cpos = length
            }
        }
    }
}
