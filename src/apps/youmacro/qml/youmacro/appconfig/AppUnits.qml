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
    
    function ps(x) {
    	// On a windows 10 desktop where I'm developing this app,
    	// the physical dots per inch is 108.888.
    	// On a surface pro 3 it's 216.213.
    	// On macos its 305.
    	return x * base_physical_dots_per_inch / physical_dots_per_inch; // physical_dots_per_inch is fed in from the c++ side.
    }
}