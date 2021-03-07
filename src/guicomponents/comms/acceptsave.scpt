
# Our debug output.
set output to ""

tell application "System Events"
    repeat with chrome_process in (every process whose name is "Google Chrome") 

        # Make this the active process.
        #set the frontmost of chrome_process to true

        # Get the pid.
        set pid to the unix id of chrome_process
        set output to output & "pid=" & pid & "\n"

        # Work on the process.
        tell chrome_process
            #activate
            if count of windows > 0 then
                set window_name to name of front window
                set output to output & "window name=" & window_name & "\n"
                if window_name equals "Save" then
                    click button "Save" of front window
                    # Check if we need to confirm, when the file already exists.
                    if count of windows > 0 then
                        set window_name2 to name of front window
                        if window_name2 equals "Save" then
                            click button "Replace" of sheet 1 of window 1
                        end if
                    end if
                end if
            end if
        end tell
    end repeat
end tell

# Return our debug output.
return output 

