import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Text  {
    id: wrapping_text
    text: ""
    
    // Font.
    font.pointSize: app_settings.large_font_point_size
    font.bold: false
    font.italic: false
    
    // Appearnace.
    color: "white"
    
    // Alignment.
    horizontalAlignment: Text.AlignHCenter
    
    // Wrapping.
    wrapMode: Text.Wrap
    maximumLineCount: 6 // The maximum number of lines to show.
}
