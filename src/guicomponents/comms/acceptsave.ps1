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
    public static extern int SendMessage(int hWnd, int hMsg, int wParam, int lParam);

    [DllImport("user32.dll")]
    public static extern int PostMessage(int hWnd, int hMsg, int wParam, int lParam);

    [DllImport("user32.dll")]
    public static extern void keybd_event(int bVk, int bScan, int dwFlags, int dwExtraInfo);

    [DllImport("user32.dll")]
    public static extern IntPtr GetDlgItem(IntPtr hWnd, int nIDDlgItem);

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

# Some constants.
$WM_NEXTDLGCTL = 0x0028
$WM_KEYDOWN = 0x0100
$WM_KEYUP = 0x0101
$WM_SYSKEYDOWN = 0x104
$WM_SYSKEYUP = 0x105
$WM_LBUTTONDOWN = 0x0201
$WM_LBUTTONUP = 0x0202
$WM_CHAR = 0x0102
$WM_COMMAND = 0x0111

$MK_LBUTTON = 0x0001

$VK_RETURN = 0x0D
$VK_TAB = 0x09

$BN_CLICKED = 0
$SaveButtonId = 0x01
$CancelButtonId = 0x02
$YesButtonId = 0x01
$NoButtonId = 0x00

# These are the main windows of the non-robodownloader and robodownloader windows of chrome.
$main_chrome_handles = Get-Process "chrome" | ?{$_.MainWindowTitle} | %{$_.MainWindowHandle}
"main chrome handles: " + $main_chrome_handles

# The other pids are of the tabs inside the main windows.
$all_chrome_pids = Get-Process "chrome" | %{$_.ID}

# ----------------------------------------------------------------------------------------------
# It doesn't really matter which pid we choose to start as we're only using it
# to get a window handle from which we start iterating through all the window handles.
# ----------------------------------------------------------------------------------------------

$window_handle = $main_chrome_handles[1]
foreach ($start_handle in $main_chrome_handles) {
$window_handle = $start_handle

while ($window_handle -ne 0) {

    # Get process id and thread id from the window handle.
    $process_id = 0
    $thread_id = [UserWindows]::GetWindowThreadProcessId($window_handle, [ref] $process_id)
    $process = Get-Process -Id $process_id

    if ($all_chrome_pids -contains $process_id) {

        # Determine the window title. 
        $window_name = Get-WindowName $window_handle
        "window name: " + $window_name
        
        # If the window name is Save As.
        if ($window_name -eq "Save As") {
                
            # Find and click the yes button.
            $child_handles = [UserWindows]::GetChildWindows($window_handle)
            foreach($child_handle in $child_handles)
            {
                $cname = Get-WindowName($child_handle)
                if ($cname -eq "&Save") {
                    [UserWindows]::PostMessage($child_handle, $WM_LBUTTONDOWN, $MK_LBUTTON, 0);
                    [UserWindows]::PostMessage($child_handle, $WM_LBUTTONUP, $MK_LBUTTON, 0);
                }
            }

            # Wait a tiny bit just in case the Confirm Save As dialog pops up.
            Start-Sleep -Milliseconds 1000 

            # If the Save As dialog is still up,
            # send keys tab and enter as the Confirm Save As dialog should be focused.
            # Todo: Try to find the Confirm Save As window directly. For some reason it doesn't show up under its own process.
            if (!$process.HasExited) {

                # Get the popup handle.
                $popup_handle = [UserWindows]::GetWindow($window_handle, 6)
                "popup handle is: " + $popup_handle

                # Get the popup name.
                $popup_name = Get-WindowName $popup_handle
                "popup name is: " + $popup_name
                
                # Find and click the yes button.
                $child_handles = [UserWindows]::GetChildWindows($popup_handle)
                foreach($child_handle in $child_handles)
                {
                    $cname = Get-WindowName($child_handle)
                    if ($cname -eq "&Yes") {
                        [UserWindows]::PostMessage($child_handle, $WM_LBUTTONDOWN, $MK_LBUTTON, 0);
                        [UserWindows]::PostMessage($child_handle, $WM_LBUTTONUP, $MK_LBUTTON, 0);
                    }
                }

                # Wait a tiny bit for the Confirm Save As dialog to go away.
                Start-Sleep -Milliseconds 1000 
            }
        }
    }

    # Go to the next window handle. 
    $window_handle = [UserWindows]::GetWindow($window_handle, 2)
}
}
"Done!"
#Read-Host -Prompt "Press Enter to exit: "
