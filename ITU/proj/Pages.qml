import QtQuick 2.9

Item {
    id: root

    property alias text: pages.text
    property alias leftbuttop: buttleft.opacity
    property alias rightbuttop: buttright.opacity
    signal prev()
    signal next()

    Row {
        spacing: 16
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top

        Button {
            id: buttleft
            width: 32
            height: width
            text: "<"
            textbold: true;
            textpixelsize: 22;
            onClick: {
                root.prev()
            }
        }

        Text {
            id: pages
            width: 32
            height: width
            text: "1/1"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Button {
            id: buttright
            width: 32
            height: width
            text: ">"
            textbold: true;
            textpixelsize: 22;
            onClick: {
                root.next()
            }
        }
    }
}
