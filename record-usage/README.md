# record-usage

## Description:
Record which app the current user is using, and for how long.

Every 10 seconds, it wakes up to check the foreground window's title, the application that the window belongs to, and then writes the info to the log file. If the user is away or power / display is off, the recording will be paused.

## Setup instruction:

 - copy record-usage.exe C:\windows\system32
 - reg add HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v record-usage /d C:\windows\system32\record-usage.exe

## Log file:
 - Location: %LOCALAPPDATA%\record-usage.csv
 - Each line contains 4 columns: timestamp, duration (in seconds), command line, window title. For example:
 - `2017-01-27 17:50:51, 60, "C:\Windows\SystemApps\Microsoft.MicrosoftEdge_8wekyb3d8bbwe\MicrosoftEdge.exe", Google and 2 more pages`

