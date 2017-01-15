[CmdletBinding()]
Param(
[string]$n,[string]$interval
)
Add-Type @"
  using System;
  using System.Runtime.InteropServices;
  public class UserWindows {

    [DllImport("user32.dll")]
    public static extern IntPtr GetForegroundWindow();
    
    [DllImport("user32.dll")]
    public static extern int GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

    [DllImport("user32.dll")]
    public static extern IntPtr GetWindowText(IntPtr hWnd, System.Text.StringBuilder text, int count);

    [DllImport("user32.dll")]
    public static extern IntPtr GetWindowTextLength(IntPtr hWnd);
}
"@

$stringbuilder = New-Object System.Text.StringBuilder 256

try {
	while($n -ne 0){
        # Get the active window handle.
		$active_handle = [UserWindows]::GetForegroundWindow()

        # Determine the process id and thread id.
        $process_id = 0
        $thread_id = [UserWindows]::GetWindowThreadProcessId($active_handle, [ref] $process_id)

        # Determine the window title. 
        $window_title = ""
        $count = [UserWindows]::GetWindowText($active_handle, $stringbuilder, 256)
        if (0 -lt $count) {
            $window_title = $stringbuilder.ToString()
        }

        # Grab the process info.
		$process_info = Get-Process | ? {$_.Id -eq $process_id}

        # Build return value.
        $string = $process_info | Select ProcessName, @{Name="AppTitle";Expression= {($window_title)}}
		Write-Host -NoNewline $string
		#Write-Host $string

        # Set up for the next iteration.
		Start-Sleep -s $interval
		If ($n -gt 0) {$n-=1} 
	}
} catch {
 Write-Error "Failed to get active Window details. More Info: $_"
}

#Read-Host -Prompt "Press Enter to exit"
