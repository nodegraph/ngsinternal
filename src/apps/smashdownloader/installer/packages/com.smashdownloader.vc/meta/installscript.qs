function Component()
{
    if (systemInfo.kernelType === "winnt") {
    	console.log("OS: " + systemInfo.productType);
    	console.log("installing msvc redistributable");
    	var program = installer.value("TargetDir");
    	console.log("target dir: " + program);
    	program = program.replace(/\//g,"\\");
    	console.log("target dir with forward slashes replaced: " + program);
    	program += "\\bin\\vc_redist.x64.exe";
    	console.log("the vc runtime program is: " + program);
    	component.addElevatedOperation("Execute", program, "/install", "/passive", "/norestart");
    } else if (systemInfo.kernelType === "darwin") {
    } else {
		cancelInstaller("Installation on " + systemInfo.prettyProductName + " is not supported");
    }
}