function Component()
{
    //if (systemInfo.kernelType === "winnt") {
    //} else if (systemInfo.kernelType === "darwin") {
    
    
    if (systemInfo.productType === "windows") {
    	console.log("installing msvc redistributable");
    	component.addElevatedOperation("Execute", "@TargetDir@\\bin\\vc_redist.x64.exe", "/install", "/passive", "/norestart");
    } else {
		cancelInstaller("Installation on " + systemInfo.prettyProductName + " is not supported");
    }
}