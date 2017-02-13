import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

RowLayout {
	anchors.horizontalCenter: parent.horizontalCenter
	property alias text: password_field.text
	property alias password_field: password_field

	TextField {
	    id: password_field
	    
	    // Dimensions.
	    height: app_settings.text_field_height
	    width: app_settings.page_width * 0.8
	
	    // Input.
	    inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhHiddenText
	    text: ""
	    textColor: "white"
	    font.pointSize: app_settings.font_point_size
	    echoMode: TextInput.Password 
	
	    // Center the text.
	    horizontalAlignment: TextInput.AlignHCenter
	    verticalAlignment: TextInput.AlignVCenter
	    
	    function reveal() {
	    	echoMode = TextInput.Normal 
	    }
	    
	    function hide() {
	    	echoMode = TextInput.Password 
	    }
	    
	    // Our Style.
	    style: TextFieldStyle {
	
	        // Text Properties.
	        textColor: "white"
	        font.pointSize: app_settings.font_point_size
	
	        // Background with border.
	        background: Rectangle {
	            radius: app_settings.text_field_radius
	            height: app_settings.text_field_height
	            border.color: "#FFFFFFFF"
	            border.width: app_settings.text_field_border_size
	            color: app_settings.prompt_color
	        }
	    }
	}
	
	// Reveal Checkbox.
	AppCheckBox {
	    id: reveal_check_box
	    checked: false
	    anchors {
	    	left: password_field.right
	        leftMargin: app_settings.page_left_margin / 2
	        //rightMargin: app_settings.page_right_margin
	    }
	    onClicked: {
	    	if (checked) {
	    		password_field.reveal()
	    	} else {
	    		password_field.hide()
	    	}
	    }
	}

}
