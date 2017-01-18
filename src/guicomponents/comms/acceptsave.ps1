Add-Type @"
  using System;
  using System.Runtime.InteropServices;
  using System.Collections.Generic;
  using System.Text;
  public class UserWindows {

    [DllImport("user32.dll")]
    public static extern IntPtr GetForegroundWindow();
    
    [DllImport("user32.dll")]
    public static extern int GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

    [DllImport("user32.dll")]
    public static extern IntPtr GetWindowText(IntPtr hWnd, System.Text.StringBuilder text, int count);

    [DllImport("user32.dll")]
    public static extern IntPtr GetWindowTextLength(IntPtr hWnd);

    [DllImport("user32.dll", CharSet=CharSet.Auto)]
    public static extern IntPtr FindWindow(string className, string windowName);

    [DllImport("user32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern IntPtr GetTopWindow(IntPtr hWnd);

    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr GetWindow(IntPtr hWnd, uint uCmd);

    public enum GetWindow_Cmd : uint {
        GW_HWNDFIRST = 0,
        GW_HWNDLAST = 1,
        GW_HWNDNEXT = 2,
        GW_HWNDPREV = 3,
        GW_OWNER = 4,
        GW_CHILD = 5,
        GW_ENABLEDPOPUP = 6
    }

 	[DllImport("user32")]
   	[return: MarshalAs(UnmanagedType.Bool)]
   	public static extern bool EnumChildWindows(IntPtr window, EnumWindowProc callback, IntPtr i);
    public static List<IntPtr> GetChildWindows(IntPtr parent)
   {
       List<IntPtr> result = new List<IntPtr>();
       GCHandle listHandle = GCHandle.Alloc(result);
       try
       {
           EnumWindowProc childProc = new EnumWindowProc(EnumWindow);
           EnumChildWindows(parent, childProc, GCHandle.ToIntPtr(listHandle));
       }
       finally
       {
           if (listHandle.IsAllocated)
               listHandle.Free();
       }
       return result;
   }
    private static bool EnumWindow(IntPtr handle, IntPtr pointer)
   {
       GCHandle gch = GCHandle.FromIntPtr(pointer);
       List<IntPtr> list = gch.Target as List<IntPtr>;
       if (list == null)
       {
           throw new InvalidCastException("GCHandle Target could not be cast as List<IntPtr>");
       }
       list.Add(handle);
       //  You can modify this to check to see if you want to cancel the operation, then return a null here
       return true;
   }
    public delegate bool EnumWindowProc(IntPtr hWnd, IntPtr parameter);

}
"@

Add-Type -AssemblyName System.Windows.Forms

# Function to get window text from a window handle.
function Get-WindowName {
    param($hwnd)
    $len = [UserWindows]::GetWindowTextLength($hwnd)
    if($len -ne 0)
    {
        $sb = New-Object text.stringbuilder -ArgumentList ($len + 1)
        $rtnlen = [UserWindows]::GetWindowText($hwnd,$sb,$sb.Capacity)
        $sb.tostring()
    }
}

# These are the main windows of the non-smashbrowse and smashbrowse windows of chrome.
$main_chrome_handles = Get-Process "chrome" | ?{$_.MainWindowTitle} | %{$_.MainWindowHandle}
"main chrome handles: " + $main_chrome_handles

# The other pids are of the tabs inside the main windows.
$all_chrome_pids = Get-Process "chrome" | %{$_.ID}

# ----------------------------------------------------------------------------------------------
# It doesn't really matter which pid we choose to start as we're only using it
# to get a window handle from which we start iterating through all the window handles.
# ----------------------------------------------------------------------------------------------

$window_handle = $main_chrome_handles[0]
while ($window_handle -ne 0) {

    # Get process id and thread id from the window handle.
    $process_id = 0
    $thread_id = [UserWindows]::GetWindowThreadProcessId($window_handle, [ref] $process_id)
    $process = Get-Process -Id $process_id

    if ($all_chrome_pids -contains $process_id) {

        # Determine the window title. 
        $window_name = Get-WindowName $window_handle

        # If the window name is Save As.
        if ($window_name -eq "Save As") {
            "found save as window with pid: " + $process_id
            $foreground_window_set = [UserWindows]::SetForegroundWindow($window_handle)
            [System.Windows.Forms.SendKeys]::SendWait("{ENTER}")

            Start-Sleep -Milliseconds 1000

            # If the Save As dialog is still up,
            # send keys tab and enter as the Confirm Save As dialog should be focused.
            # Todo: Try to find the Confirm Save As window directly. For some reason it doesn't show up under its own process.
            if (!$process.HasExited) {
                [System.Windows.Forms.SendKeys]::SendWait("{TAB}")
                [System.Windows.Forms.SendKeys]::SendWait("{ENTER}")
            }
        }
    }

    # Go to the next window handle. 
    $window_handle = [UserWindows]::GetWindow($window_handle, 2)
}

#Read-Host -Prompt "Press Enter to exit: "
