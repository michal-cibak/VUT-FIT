import QtQuick 2.9

Item {
    id: root
    width: 104
    height: 90

    property int importance: 0
    property alias date: date_label.text
    property alias datecolor: date_label.color
    property alias text: note.text
    property alias visibility: reminder.visible
    signal click()

    Rectangle {
        id: reminder
        width: root.width
        height: root.height
        property variant colors: ['greenyellow', 'yellow', 'orange']

        Column {
            Rectangle {
                id: date
                width: root.width
                height: 16
                color: reminder.colors[importance]
                border.width: 2
                border.color: Qt.darker(color)

                Text {
                    id: date_label
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font.bold: true
                    text: "DD. MM. RRRR"
                }
            }

            Note {
                id: note
                width: root.width
                height: root.height - date.height
                color: reminder.colors[importance]
                text: "I am a reminder."
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.click()
            }
        }
    }
}
