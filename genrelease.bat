@echo off
setlocal

REM Create a unique timestamp for the filename
for /f %%x in ('powershell -command "Get-Date -UFormat %%s"') do set timestamp=%%x
set filename=release_%timestamp%.zip

REM Create the releases folder if it doesn't exist
if not exist "releases" mkdir "releases"

REM Create tmpFolder/apps/boot.dol directory structure
mkdir "tmpFolder\apps"

REM Copy carhorn.dol to tmpFolder/apps/boot.dol
copy "carhorn.dol" "tmpFolder\apps\boot.dol"

REM Copy contents of HBApp/* to tmpFolder/apps/
xcopy /s /e "HBApp\*" "tmpFolder\apps\"

REM Zip tmpFolder to releases\release_{timestamp}.zip with root as apps/
powershell Compress-Archive -Path "tmpFolder\*" -DestinationPath "releases\%filename%" -CompressionLevel Optimal

REM Delete tmpFolder
rmdir /s /q "tmpFolder"

echo Done. Created: %filename%
pause