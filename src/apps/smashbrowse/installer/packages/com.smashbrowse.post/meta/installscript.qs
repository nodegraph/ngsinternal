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
        	"@TargetDir@/bin/smashbrowse.exe", 
        	"@StartMenuDir@/smashbrowse.lnk",
            "workingDirectory=@TargetDir@/bin", 
            "iconPath=@TargetDir@/bin/octopus_blue.ico");
            
        // Create a shortcut on the desktop.
        component.addOperation("CreateShortcut", 
        	"@TargetDir@/bin/smashbrowse.exe", 
        	"@DesktopDir@/smashbrowse.lnk",
        	"workingDirectory=@TargetDir@/bin", 
            "iconPath=@TargetDir@/bin/octopus_blue.ico");
            
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
	    //        var argList = ["smashbrowse", "/D", "@TargetDir@/bin/", "/B", "@TargetDir@/bin/smashbrowse.exe"];
	    //        installer.execute("start", argList);
	    //    }
	    //} catch(e) {
	    //    console.log(e);
	    //}
	} else {
	    //try {
	    //    if (installer.isInstaller() && installer.status == QInstaller.Success) {
	    //        var argList = ["-a", "@TargetDir@/smashbrowse.app"];
	    //        installer.execute("open", argList);
	    //    }
	    //} catch(e) {
	    //    console.log(e);
	    //}
    }
}
