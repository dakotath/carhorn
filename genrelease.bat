@echo off
setlocal

REM Create the releases folder if it doesn't exist
if not exist "releases" mkdir "releases"

REM Create tmpFolder/apps/carhorn directory structure
mkdir "tmpFolder\apps\carhorn"

REM Copy carhorn.dol to tmpFolder/apps/carhorn/boot.dol
copy "carhorn.dol" "tmpFolder\apps\carhorn\boot.dol"

REM Copy contents of HBApp/* to tmpFolder/apps/carhorn/
xcopy /s /e "HBApp\*" "tmpFolder\apps\carhorn\"

REM Delete old release.
del "releases\carhorn.zip"

REM Zip tmpFolder/apps/carhorn to releases\carhorn.zip
powershell Compress-Archive -Path "tmpFolder\*" -DestinationPath "releases\carhorn.zip" -CompressionLevel Optimal

REM Delete tmpFolder
rmdir /s /q "tmpFolder"

echo Done.
pause
