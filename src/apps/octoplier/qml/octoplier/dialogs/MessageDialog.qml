import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

MessageDialog {
    id: message_dialog
    title: qsTr("May I have your attention, please?")
    height: 100

    function show(caption) {
        message_dialog.text = caption
        message_dialog.open()
    }
}
