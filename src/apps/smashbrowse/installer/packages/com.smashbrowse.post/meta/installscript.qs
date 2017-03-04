function Component()
{
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
    }
}