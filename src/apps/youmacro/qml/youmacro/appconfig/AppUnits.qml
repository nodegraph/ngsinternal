import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Controls.Private 1.0

import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4


QtObject {
    function dp( x ) {
        return Math.round( x * Settings.dpiScaleFactor );
    }

    function em( x ) {
        return Math.round( x * TextSingleton.font.pixelSize );
    }
}