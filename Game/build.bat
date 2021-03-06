@echo off
echo In case of error when accessing WED, adjust path to your Acknex8 installation
echo In case of access violation most likely "media" folder cannot be renamed.
echo This can happen if the game crashed unexpectedly. Restart PC and rerun build.bat
echo.

rem adjust path to engine
Set "AckPath=c:\Program Files (x86)\gstudio8"
Set "AckPath2=d:\apps\acknex8" 

rem names adjust to project
Set "main=lottervstheworld2.c"

rem setup environment
for %%i in ("%main%") do (
	Set "buildfolder=%%~ni.cd"
)
Set "path=%path%;%AckPath%;%AckPath2%"

rem standard wrs build (do not change)
if exist %buildfolder% ( rd %buildfolder% /S /Q )
ren media media_renamed_by_buildscript

rem config patch
ren config.h config.h.orig
echo // kommentar > config.h

rem atlas map patch
del level\room_atlas_01.pcx
copy level\BUILD_room_atlas_01_big.pcx level\room_atlas_01.pcx

timeout /t 1 /nobreak > NUL
wed -r %main% > build.log
ren media_renamed_by_buildscript media

rem specific files to be copied (adjust to project)
xcopy media %buildfolder%\media\ /Y >>build.log
xcopy readme.txt %buildfolder% /Y >>build.log
xcopy credits.dat %buildfolder% /Y >>build.log
rem xcopy acknex.wdf %buildfolder% /Y >>build.log
xcopy ackxinput.dll %buildfolder% /Y >>build.log
rem xcopy fails this Oo
copy disabled.wdf %buildfolder%\acknex.wdf /Y >>build.log

rem cleanup
for %%i in ("%main%") do (
	del %%~ni.exe
)

rem revert config patch
del config.h
ren config.h.orig config.h

rem revert atlas map patch
del level\room_atlas_01.pcx
copy level\BUILD_room_atlas_01_small.pcx level\room_atlas_01.pcx

echo Build finished. See build.log for any errors.
echo.

pause
