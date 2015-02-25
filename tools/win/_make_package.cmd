@echo off
rem make package tool for obs-rtmp-nicolive

set OBS_RTMP_NICOLIVE_VERSION=1.1
set PACAKGE_NAME=obs-rtmp-nicolive_1.1_win

set BUILD32_DIR=..\build32
set BUILD64_DIR=..\build64
set DATA_DIR=..\data
set PLUGIN_NAME=rtmp-nicolive

set SEVEN_ZIP_EXE=C:\Program Files\7-Zip\7z.exe
set RUBY_EXE=ruby.exe

set README_MD=..\README.md
set WIN_INSTALL_MD=..\tools\win\WIN_INSTALL.md
set CONVERT_README_RB=..\tools\win\convert_readme.rb

"%SEVEN_ZIP_EXE%"
if errorlevel 1 echo "Failed check 7-Zip. Please install 7-Zip or change SEVEN_ZIP_EXE"
"%RUBY_EXE%" -v
if errorlevel 1 echo "Failed check Ruby. Please install Ruby and set PATH"

echo creating package...

if exist "%PACAKGE_NAME%" rmdir /s /q "%PACAKGE_NAME%"
if exist "%PACAKGE_NAME%.7z" rmdir /s /q "%PACAKGE_NAME%.7z"

mkdir "%PACAKGE_NAME%"

mkdir "%PACAKGE_NAME%\obs-plugins"
mkdir "%PACAKGE_NAME%\obs-plugins\32bit"
mkdir "%PACAKGE_NAME%\obs-plugins\64bit"
mkdir "%PACAKGE_NAME%\data"
mkdir "%PACAKGE_NAME%\data\obs-plugins"

copy /b /y "%BUILD32_DIR%\Relese\%PLUGIN_NAME%.dll" "%PACAKGE_NAME%\obs-plugins\32bit\%PLUGIN_NAME%.dll"
copy /b /y "%BUILD64_DIR%\Relese\%PLUGIN_NAME%.dll" "%PACAKGE_NAME%\obs-plugins\64bit\%PLUGIN_NAME%.dll"
xcopy "%DATA_DIR%" "%PACAKGE_NAME%\data\obs-plugins\%PLUGIN_NAME%" /s
"%RUBY_EXE%" "%CONVERT_README_RB%" "%README_MD%" "%WIN_INSTALL_MD%" README.txt
"%SEVEN_ZIP_EXE%" a -r "%PACAKGE_NAME%.7z" "%PACAKGE_NAME%"

echo ##### Succeeded to create package. #####
echo You read %PACAKGE_NAME%\README.txt if you want to install this plugin.
echo And you can deploy the %PACAKGE_NAME%.7z file!

goto :eof

:die
set message=%~1
if "a%~2a" == "aa" set code=1 else set /a code=%~2
echo %message%
if "%code" equ "255" pause
exit /b %code%
goto :eof
