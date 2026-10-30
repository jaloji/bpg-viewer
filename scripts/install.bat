@echo off
rem Installs the BPG WIC codec (Windows Explorer thumbnails for BPG images).
rem Run as administrator from the release folder (BpgWic.dll next to this file).
setlocal
cd /d "%~dp0"

net session >nul 2>&1 || (
  echo Please run this file as administrator.
  exit /b 1
)
if not exist BpgWic.dll (
  echo BpgWic.dll not found next to install.bat. Build the project first, then run dist\install.bat.
  exit /b 1
)

set "APPDIR=%ProgramFiles%\BpgViewer"

if not exist "%APPDIR%" mkdir "%APPDIR%"
if exist "%APPDIR%\BpgWic.dll" regsvr32 /s /u "%APPDIR%\BpgWic.dll"
copy /y BpgWic.dll "%APPDIR%\BpgWic.dll" >nul || (
  echo Could not replace "%APPDIR%\BpgWic.dll": it is probably in use by Explorer.
  echo Run uninstall.bat, sign out or restart, then run install.bat again.
  exit /b 1
)
regsvr32 /s "%APPDIR%\BpgWic.dll" || (
  echo Codec registration failed. If Microsoft Defender attack surface reduction rules are
  echo enforced on this PC, allow the DLL first ^(admin PowerShell^):
  echo   Add-MpPreference -AttackSurfaceReductionOnlyExclusions "%APPDIR%\BpgWic.dll"
  exit /b 1
)
echo BPG codec installed: %APPDIR%\BpgWic.dll
echo.
echo Done. If .bpg files still show blank icons, run clear-thumbnails.bat.
