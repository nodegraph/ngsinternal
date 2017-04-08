import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

Item{
    id: app_settings
    objectName: "app_settings_object"
    visible: false

    function try_catch_wrap(f) {
        return function() {
            try {
                f.apply(this, arguments)
            } catch(e) {
                //console.log("Error: caught exception: " + e)
                console.log("Error: caught exeption: " + e.message + "\nstacktrace: " + e.stack)
            }
        }
    }

    // -------------------------------------------------------------------
    // The 3 main sections of our gui.
    // -------------------------------------------------------------------
        
    // Action Bar Properties.
    property color action_bar_bg_color: "#FF0288D1" // a darker blue than the node graph blue
    property color action_bar_pressed_color: ng_bg_alternate_color //"#FF212121"
    
    property int action_bar_left_margin:  app_units.dp(10)
    property int action_bar_right_margin: app_units.dp(10)
    
    property int page_left_margin:  app_units.dp(30)
    property int page_right_margin: app_units.dp(30)
    property int page_content_width: page_width - page_left_margin - page_right_margin
    property int page_content_left: page_left_margin
    property int page_content_right: page_width - page_right_margin
    
    property int action_bar_height: app_units.dp(60)
    property int action_bar_width: parent.width
    property int action_bar_x: 0
    property int action_bar_y: 0
    property int action_bar_z: 0

    // List View Properties.
    property int list_item_height_large: app_units.dp(120)
    property int list_item_height_small: app_units.dp(60)
    property int list_item_border_width: app_units.dp(1)

    // Menu Stack Properties.
    property color menu_stack_bg_color: action_bar_bg_color //"#FF0288D1"
    property color menu_page_bg_color: action_bar_bg_color //"#FF0288D1"
    property color menu_stack_header_bg_color: "#FF4CAF50" // a light green color
    property color menu_stack_header_border_color: "#FFFFFFFF" // pure white
    property int menu_stack_header_border_width: app_units.dp(2)
    property double menu_stack_header_radius: app_units.dp(0)
    
    // Splitter Properties.
    property int splitter_height: app_units.dp(2)
    property int splitter_width: parent.width
    property int splitter_x: 0
    property int splitter_y: action_bar_height
    property int splitter_z: 0

    // Screen Properties.
    property int screen_height: parent.height
    property int screen_width: parent.width
    
    // Page Properties.
    property int page_height: (parent.height - action_bar_height - splitter_height)
    property int page_width: parent.width
    // These values are with respect to the whole screen.
    property int page_x: 0
    property int page_y: action_bar_height + splitter_height
    property int page_z: 0
    property int column_layout_spacing: app_units.dp(20)

    // Menu Page Properties.
    property int menu_page_height: page_height - action_bar_height
    property int menu_page_width: page_width
    // These values are with respect to the menu page.
    property int menu_page_x: 0
    property int menu_page_y: action_bar_height
    property int menu_page_z: 0
    property double menu_page_title_point_size: 22
    property double menu_page_description_point_size: 19
    property double download_title_point_size: 12
    property double download_description_point_size: 10
    
    // -------------------------------------------------------------------
    // Modes
    // -------------------------------------------------------------------
    
    property int file_mode: 0
    property int node_graph_mode: 1
    property int view_node_mode: 2
    property int edit_node_mode: 3
    property int downloads_mode: 4
    property int downloaded_mode: 5
    property int posts_mode: 6
    property int settings_mode: 7
    
    // -------------------------------------------------------------------
    // Layering.
    // -------------------------------------------------------------------
    
    property int copy_paste_bar_z: 10
    property int app_tooltip_z: 20
    
    // -------------------------------------------------------------------
    // Other sub component properties.
    // -------------------------------------------------------------------

    // General UI properties.
    property color prompt_color: "#FF01579B" // a dark blue color that prompts for user input
    
    // Node Graph.
    // The node graph bg color (3.0f/255.0f, 169.0f/255.0f, 244.0f/255.0f, 1.00).
    property color ng_bg_color: "#FF03A9F4" // a darker blue
    property color ng_bg_alternate_color: "#FF29B6F6" // a lighter blue
    property color ng_bg_alternate_transparent_color: "#9929B6F6" // a transparent ligher blue
        
    // Font Properties.
    property double large_font_point_size: 30
    property double font_point_size: 25
    property double tree_view_font_point_size: 25

    // Image Button Properties.
    property int image_button_width: app_units.dp(50)
    property int image_button_height: app_units.dp(50)
    property color image_button_press_color: "#FF4CAF50" // a light green color
    property int image_button_radius: app_units.dp(10)

    // Button Spacing.
    property int button_spacing: app_units.dp(20)
    
    // Text Field Properties.
    property int text_field_height: app_units.dp(50)
    property double text_field_border_size: app_units.dp(2)
    property double text_field_radius: app_units.dp(10)

    // CheckBox Properties.
    property int check_box_radius: app_units.dp(3)
    property int check_box_border_width: app_units.dp(3)
    property int check_box_size: app_units.dp(40)
    
    // Tree View.
    property double tree_view_header_height: app_units.dp(60)
    property double tree_view_row_height: app_units.dp(60)
    property double tree_view_indentation: app_units.dp(60)
    
    property double tree_view_twisty_width: app_units.dp(60)
    property double tree_view_twisty_height: app_units.dp(60)
    
    property double tree_view_header_left_margin:  app_units.dp(15)
    
    property color tree_view_item_text_color: "#FFFFFFFF"
    property color tree_view_selected_row_bg_color: "#FF4CAF50" // a light green color

    // Tooltip Properties.
    property color tooltip_bg_color: "#FF4CAF50" // a light green color
        
    Component.onCompleted: {
    }
}

