@echo off

REM this determines VISUAL_STUDIO_DIR, the location of the visual studio command line build tools
REM Can be installed from: https://visualstudio.microsoft.com/visual-cpp-build-tools/
set VISUAL_STUDIO_DIR=
echo VISUAL_STUDIO_DIR :: %VISUAL_STUDIO_DIR%
set VISUAL_STUDIO_DIR_OPTION_1=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools
set VISUAL_STUDIO_DIR_OPTION_2=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional
set VISUAL_STUDIO_DIR_OPTION_3=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
IF NOT EXIST "%VISUAL_STUDIO_DIR_OPTION_1%" goto :SKIP_OPT_1
set VISUAL_STUDIO_DIR=%VISUAL_STUDIO_DIR_OPTION_1%
:SKIP_OPT_1
IF NOT EXIST "%VISUAL_STUDIO_DIR_OPTION_2%" goto :SKIP_OPT_2
set VISUAL_STUDIO_DIR=%VISUAL_STUDIO_DIR_OPTION_2%
:SKIP_OPT_2
IF NOT EXIST "%VISUAL_STUDIO_DIR_OPTION_3%" goto :SKIP_OPT_3
set VISUAL_STUDIO_DIR=%VISUAL_STUDIO_DIR_OPTION_3%
:SKIP_OPT_3
echo VISUAL_STUDIO_DIR :: %VISUAL_STUDIO_DIR%
set MSVC_CMAKE="%VISUAL_STUDIO_DIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set MSVC_ENV="%VISUAL_STUDIO_DIR%\VC\Auxiliary\Build\vcvars64.bat"

