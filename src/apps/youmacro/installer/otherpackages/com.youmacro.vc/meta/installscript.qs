function Component()
{
    if (systemInfo.kernelType === "winnt") {
    	console.log("installing msvc redistributable");
    	component.addElevatedOperation("Execute", "@TargetDir@\\bin\\vc_redist.x64.exe", "/install", "/passive", "/norestart");
    } else if (systemInfo.kernelType === "darwin") {
    } else {
		cancelInstaller("Installation on " + systemInfo.prettyProductName + " is not supported");
    }
}