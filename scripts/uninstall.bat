@echo off
rem Removes the BPG WIC codec.
rem Run as administrator.
setlocal

net session >nul 2>&1 || (
  echo Please run this file as administrator.
  exit /b 1
)

set "APPDIR=%ProgramFiles%\BpgViewer"

if exist "%APPDIR%\BpgWic.dll" (
  regsvr32 /s /u "%APPDIR%\BpgWic.dll"
  rem Explorer may still hold the DLL; it can then be deleted after a restart.
  del /f "%APPDIR%\BpgWic.dll" 2>nul
  if exist "%APPDIR%\BpgWic.dll" echo BpgWic.dll is in use, delete "%APPDIR%" after a restart.
  rmdir "%APPDIR%" 2>nul
  echo BPG codec removed.
)
