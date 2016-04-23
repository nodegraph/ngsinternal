import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import todopile.tools 1.0 

AppMenu {
    // Public Properties.
    id: node_graph_menu
    title: "Edit Node Graph"
    property bool center_new_nodes: false
    
    // Methods.
    function on_popup_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        popup()
    }
    function on_popup_menu_with_centering() {
        app_settings.vibrate()
        center_new_nodes = true
        popup()
    }

//    MenuItem {
//        text: "test restoring graph"
//        onTriggered: {
//            node_graph_page.node_graph.restore_default_node_graph();
//        }
//    }
        
//    MenuItem {
//        text: "test saving graph"
//        onTriggered: {
//            node_graph_page.node_graph.save();
//        }
//    }

//    MenuItem {
//        text: "test loading graph"
//        onTriggered: {
//            node_graph_page.node_graph.load();
//        }
//    }
    
    MenuItem {
        text: "Surface To Parent Group"
        onTriggered: {
            node_graph_page.node_graph.surface();
        }
    }
    
    AppMenu {
        title: "Frame Nodes..."

        MenuItem {
            text: "Frame All Nodes"
            onTriggered: {
                node_graph_page.node_graph.frame_all();
            }
        }
        
        MenuItem {
            text: "Frame Selected Nodes"
            onTriggered: {
                node_graph_page.node_graph.frame_selected();
            }
        }
    }

    AppMenu {
        title: "Select Nodes..."
        MenuItem {
            text: "Select All"
            onTriggered: {
                node_graph_page.node_graph.select_all();
            }
        }

        MenuItem {
            text: "Deselect All"
            onTriggered: {
                node_graph_page.node_graph.deselect_all();
            }
        }
    }

    AppMenu {
    	title: "Edit Graph..."

        AppMenu {
            title: "Create Node..."
    
            MenuItem {
                text: "group"
                onTriggered: {
                    node_graph_page.node_graph.create_group_node(center_new_nodes);
                }
            }
    
            MenuItem {
                text: "input"
                onTriggered: {
                    node_graph_page.node_graph.create_input_node(center_new_nodes);
                }
            }
            
            MenuItem {
                text: "output"
                onTriggered: {
                    node_graph_page.node_graph.create_output_node(center_new_nodes);
                }
            }
            
            MenuItem {
                text: "dot"
                onTriggered: {
                    node_graph_page.node_graph.create_dot_node(center_new_nodes);
                }
            }

            MenuItem {
                text: "mock"
                onTriggered: {
                    node_graph_page.node_graph.create_mock_node(center_new_nodes);
                }
            }

        }
    	    
		MenuItem {
	        text: "Copy"
	        onTriggered: {
                node_graph_page.node_graph.copy();
	        }
	    }
    
        MenuItem {
            text: "Cut"
            onTriggered: {
                node_graph_page.node_graph.cut();
            }
        }
        
        MenuItem {
            text: "Paste"
            onTriggered: {
                node_graph_page.node_graph.paste(center_new_nodes);
            }
        }
	    
	    MenuItem {
	    	text: "Delete"
            onTriggered: {
                node_graph_page.node_graph.destroy_selection();
            }
	    }
	    
        MenuItem {
            text: "Collapse to Group"
            onTriggered: {
                node_graph_page.node_graph.collapse_to_group();
            }
        }
        
        MenuItem {
            text: "Explode Group"
            onTriggered: {
                node_graph_page.node_graph.explode_group();
            }
        }
	}  
    
//    AppMenu {
//        title: "Share Graph..."
//        MenuItem {
//            text: "Export Group..."
//            onTriggered: {
//                choose_file_dialog.open()
//            }
//        }
//        MenuItem {
//            text: "Import Group..."
//        }
//    }
}
