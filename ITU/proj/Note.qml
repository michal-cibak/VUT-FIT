import QtQuick 2.9

Item {
    id: root
    width: 104
    height: 90

    property alias color: note.color
    property alias text: note_label.text
    property alias visibility: note.visible
    signal click()

    Rectangle {
        id: note
        width: root.width
        height: root.height
        color: 'lightblue'
        border.width: 2
        border.color: Qt.darker(color)

        Text {
            id: note_label
            anchors.fill: parent
            anchors.margins: 5
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            text: "I am a note."
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.click()
            }
        }
    }
}
