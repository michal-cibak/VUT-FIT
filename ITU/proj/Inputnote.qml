import QtQuick 2.9

FocusScope {
    id: root
    //double size of regular
    width: 208
    height: 180

    property alias color: note.color
    property alias text: note_label.text
    property alias length: note_label.length
    property alias cpos: note_label.cursorPosition
    signal tabpress()

    FocusScope {
        focus: true

        Rectangle {
            id: note
            width: root.width
            height: root.height
            color: 'lightblue'
            border.width: 4
            border.color: Qt.darker(color)
        }

        TextEdit {
            id: note_label
            anchors.fill: note
            anchors.margins: 10
            wrapMode: TextEdit.Wrap
            font.pixelSize: 26
            focus: true
            Keys.onTabPressed: root.tabpress()
        }
    }
}
