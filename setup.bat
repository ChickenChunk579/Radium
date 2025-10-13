@echo off
setlocal

REM Navigate to subprojects directory
cd subprojects

REM Run clone and patch scripts
call clone.bat
call patch.bat

REM Navigate to Rune/subprojects
cd Rune\subprojects

REM Run clone script there
call clone.bat

endlocal
