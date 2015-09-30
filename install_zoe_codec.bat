ECHO OFF 

ECHO Install 64 bit
rundll32.exe setupapi.dll,InstallHinfSection DefaultInstall 0 %~dp0zoecod64.inf

ECHO Install 32 bit
c:\windows\syswow64\rundll32.exe setupapi.dll,InstallHinfSection DefaultInstall 0 %~dp0zoecod32.inf

pause
