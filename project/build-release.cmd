@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "N8RO_ROOT=%SCRIPT_DIR%..\..\..\.."
set "PROJECT_FILE=%SCRIPT_DIR%sim-char-anim-nathan-arm-wave.vcxproj"
set "OUTPUT_DLL=%SCRIPT_DIR%bin\release\sim-char-anim-nathan-arm-wave.dll"
set "DEPLOY_DIR=C:\N8RO\userPlugins\sim"
set "DEPLOY_DLL=%DEPLOY_DIR%\sim-char-anim-nathan-arm-wave.dll"

REM Normalize to a single Path variable before vcvars runs.
set "ORIGINAL_PATH=%Path%"
set "PATH="
set "Path=%ORIGINAL_PATH%"

call "%N8RO_ROOT%\setup.cmd"
if errorlevel 1 exit /b 1

call "%N8RO_ROOT%\dev\setup-dev.cmd"
if errorlevel 1 exit /b 1

if not defined N8RO_RELEASE_MSBUILD_CMD (
    echo [Error] N8RO_RELEASE_MSBUILD_CMD is not defined after dev setup.
    exit /b 1
)

if not exist "%DEPLOY_DIR%" (
    mkdir "%DEPLOY_DIR%"
    if errorlevel 1 exit /b 1
)

"%N8RO_RELEASE_MSBUILD_CMD%" "%PROJECT_FILE%" /m "/t:Clean;Build" /p:Configuration=Release /p:Platform=x64 /p:N8RO_RELEASE=C:\N8RO /p:N8RO_RELEASE_USER_SIM_PLUGINS=C:\N8RO\userPlugins\sim
if errorlevel 1 exit /b 1

if not exist "%OUTPUT_DLL%" (
    echo [Error] Expected DLL was not produced: %OUTPUT_DLL%
    exit /b 1
)

copy /Y "%OUTPUT_DLL%" "%DEPLOY_DLL%" >nul
if errorlevel 1 exit /b 1

echo [OK] Built: %OUTPUT_DLL%
echo [OK] Deployed: %DEPLOY_DLL%
exit /b 0
