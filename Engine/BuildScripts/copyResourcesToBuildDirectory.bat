@echo off
goto :CopyResources
==========================================================================================
CopyResources
  Usage:
    This script sets copies the required files for the project into the "Build Directory"

  Notes:
   

==========================================================================================
:CopyResources

echo xcopy "%~dp0..\BuildDependencies\%2\%3\"*.* %1 /s /d /y /r 
xcopy "%~dp0..\BuildDependencies\%2\%3\"*.* %1 /s /d /y /r 

@echo off
