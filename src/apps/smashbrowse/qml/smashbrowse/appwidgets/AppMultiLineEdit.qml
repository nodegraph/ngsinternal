import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


Rectangle {
    width: app_settings.page_content_width;
    height: app_settings.page_height/3;
    color: app_settings.menu_stack_bg_color

    border.color: "white"
    border.width: 2
    radius: 10

    function set_text(t) {
        text_edit.text = t;
    }

    function get_text() {
        return text_edit.text;
    }

    Flickable {
         id: flick

         anchors.horizontalCenter: parent.horizontalCenter

         width: app_settings.page_content_width * 0.95;
         height: app_settings.page_height/3 * 0.95;

         contentWidth: text_edit.paintedWidth
         contentHeight: text_edit.paintedHeight
         clip: true

         function ensureVisible(r)
         {
             if (contentX >= r.x)
                 contentX = r.x;
             else if (contentX+width <= r.x+r.width)
                 contentX = r.x+r.width-width;
             if (contentY >= r.y)
                 contentY = r.y;
             else if (contentY+height <= r.y+r.height)
                 contentY = r.y+r.height-height;

             // Without this you can't click to move the cursor if the
             // flickable has been outside the first visible bounding area.
             if (text_edit.contentWidth > flick.width) {
                text_edit.width = text_edit.contentWidth
             } else {
                 text_edit.width = flick.width
             }
             if (text_edit.contentHeight > flick.height) {
                 text_edit.height = text_edit.contentHeight
             } else {
                 text_edit.height = flick.height
             }
         }

         TextEdit {
             id: text_edit
             width: flick.width
             height: flick.height

             focus: true
             wrapMode: TextEdit.WrapAnywhere
             onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
             selectByMouse: false

             // Text.
             text: "hello"
             color: "white"
             font.pointSize: app_settings.font_point_size
         }
     }
}
