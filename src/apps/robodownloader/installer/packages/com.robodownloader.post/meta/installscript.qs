function Component()
{
	installer.installationFinished.connect(this, Component.prototype.installationFinished);
}

Component.prototype.createOperations = function()
{
    // call default implementation
    component.createOperations();

    if (systemInfo.productType === "windows") {
    
    	// Create a shortcut in the start menu.
        component.addOperation("CreateShortcut", 
        	"@TargetDir@/bin/robodownloader.exe", 
        	"@StartMenuDir@/Robo Downloader.lnk",
            "workingDirectory=@TargetDir@/bin", 
            "iconPath=@TargetDir@/bin/robot_blue.ico");
            
        // Create a shortcut on the desktop.
        component.addOperation("CreateShortcut", 
        	"@TargetDir@/bin/robodownloader.exe", 
        	"@DesktopDir@/Robo Downloader.lnk",
        	"workingDirectory=@TargetDir@/bin", 
            "iconPath=@TargetDir@/bin/robot_blue.ico");
            
    } else {
    	// On macos we don't install shortcuts because the user is used
    	// dragging apps to the Applications folder for installation and there
    	// are no shortcuts/aliases created on there desktop.
    }
}

Component.prototype.installationFinished = function()
{
	if (systemInfo.productType === "windows") {
		//try {
	    //    if (installer.isInstaller() && installer.status == QInstaller.Success) {
	    //        var argList = ["robodownloader", "/D", "@TargetDir@/bin/", "/B", "@TargetDir@/bin/robodownloader.exe"];
	    //        installer.execute("start", argList);
	    //    }
	    //} catch(e) {
	    //    console.log(e);
	    //}
	} else {
	    //try {
	    //    if (installer.isInstaller() && installer.status == QInstaller.Success) {
	    //        var argList = ["-a", "@TargetDir@/robodownloader.app"];
	    //        installer.execute("open", argList);
	    //    }
	    //} catch(e) {
	    //    console.log(e);
	    //}
    }
}
