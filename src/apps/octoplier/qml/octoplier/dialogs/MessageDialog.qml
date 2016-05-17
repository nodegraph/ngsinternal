import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

MessageDialog {
    id: message_dialog
    title: qsTr("This is a message dialog.")
    //height: 100
    icon: StandardIcon.Critical

    function show(text) {
        message_dialog.text = text
        message_dialog.open()
    }
}
