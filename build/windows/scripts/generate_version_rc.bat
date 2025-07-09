@echo off
setlocal enabledelayedexpansion


REM --- CONFIG ---
set TARGET=%1
set BASE_DIR=%~dp0..\..\..\source\plugins\%TARGET%
set TARGET_DIR=%~dp0..\..\..\binaries\x64\Release
set VERSION_H=%BASE_DIR%\version.h
set RC_FILE=%TARGET_DIR%\%TARGET%.rc
set RES_FILE=%TARGET_DIR%\%TARGET%.res
REM --------------

echo [Info] TARGET      = %TARGET%
echo [Info] BASE_DIR    = %BASE_DIR%
echo [Info] VERSION_H   = %VERSION_H%
echo [Info] RC_FILE     = %RC_FILE%
echo [Info] RES_FILE    = %RES_FILE%

if not exist "%VERSION_H%" (
    echo ERROR: %VERSION_H% not found
    exit /b 1
)

REM Extract version string
for /f "tokens=3 delims== " %%A in ('findstr /C:"#define VERSION_STRING" "%VERSION_H%"') do (
    set "VERSION=%%~A"
)

if "%VERSION%"=="" (
    echo [Error] VERSION_STRING not found in %VERSION_H%
    exit /b 2
)

echo [Info] Extracted VERSION = %VERSION%

set VERSION=!VERSION:"=!
for /f "tokens=1-3 delims=." %%A in ("%VERSION%") do (
    set MAJOR=%%A
    set MINOR=%%B
    set PATCH=%%C
)

if "%MAJOR%"=="" set MAJOR=0
if "%MINOR%"=="" set MINOR=0
if "%PATCH%"=="" set PATCH=0
set BUILD=0

echo [Info] Extracted MAJOR = %MAJOR%
echo [Info] Extracted MINOR = %MINOR%
echo [Info] Extracted PATCH = %PATCH%

echo [Info] Generating RC file: "%RC_FILE%"
chcp 1252 >nul
> "%RC_FILE%" type nul
(
echo #include ^<windows.h^>
echo VS_VERSION_INFO VERSIONINFO
echo  FILEVERSION %MAJOR%,%MINOR%,%PATCH%,%BUILD%
echo  PRODUCTVERSION %MAJOR%,%MINOR%,%PATCH%,%BUILD%
echo  FILEFLAGSMASK 0x3fL
echo  FILEFLAGS 0x0L
echo  FILEOS 0x40004L
echo  FILETYPE 0x2L
echo  FILESUBTYPE 0x0L
echo BEGIN
echo     BLOCK "StringFileInfo"
echo     BEGIN
echo         BLOCK "040904b0"
echo         BEGIN
echo             VALUE "CompanyName", "Hybrid Constructs"
echo             VALUE "FileDescription", "Plugin for Resolume VJ Software"
echo             VALUE "FileVersion", "%VERSION%"
echo             VALUE "ProductVersion", "%VERSION%"
echo             VALUE "InternalName", "%TARGET%"
echo             VALUE "OriginalFilename", "FFGL%TARGET%.dll"
echo             VALUE "ProductName", "FFGL%TARGET%"
echo         END
echo     END
echo     BLOCK "VarFileInfo"
echo     BEGIN
echo         VALUE "Translation", 0x0409, 1200
echo     END
echo END
) >> "%RC_FILE%"


if exist "%RC_FILE%" (
    echo [OK] RC file created at "%RC_FILE%"
) else (
    echo [Error] RC file was not written
    exit /b 6
)
REM Compile .rc to .res using rc.exe
REM echo Compiling RC to RES: %RES_FILE%
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
REM rc.exe /nologo /fo "%RES_FILE%" "%RC_FILE%"
REM if %errorlevel% neq 0 (
REM    echo ERROR: Failed to compile resource file
REM    exit /b %errorlevel%
REM )

exit /b 0
