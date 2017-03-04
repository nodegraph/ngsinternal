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
        	"@TargetDir@/bin/smashdownloader.exe", 
        	"@StartMenuDir@/smashdownloader.lnk",
            "workingDirectory=@TargetDir@/bin", 
            "iconPath=@TargetDir@/bin/octopus_blue.ico");
            
        // Create a shortcut on the desktop.
        component.addOperation("CreateShortcut", 
        	"@TargetDir@/bin/smashdownloader.exe", 
        	"@DesktopDir@/smashdownloader.lnk",
        	"workingDirectory=@TargetDir@/bin", 
            "iconPath=@TargetDir@/bin/octopus_blue.ico");
    }
}