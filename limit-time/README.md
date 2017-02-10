# limit-time

## Description:
Limit when a given game can be started.

Once installed, when user tries to start the game, the limit-time.exe is invoked instead. It checks whether it is weekend and the time is between 9:00 am - 9:00 pm. If so, it continues to start the game; otherwise it pops a warning that the game can only be allowed on weekend and exits.

## Setup instruction:
 - copy limit-time.exe C:\windows\system32
 - reg add "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\minecraft-launcher.exe" /v Debugger /d C:\Windows\system32\limit-time.exe
