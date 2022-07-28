import QtQuick 2.9

Rectangle {
    id: root
    width: 128
    height: 32
    color: 'lavender'
    border.color: Qt.darker(color)

    property alias text: label.text
    property alias textbold: label.font.bold
    property alias textpixelsize: label.font.pixelSize
    signal click()

    Text {
        id: label
        anchors.centerIn: parent
        text: "Button"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.click()
        }
    }
}
