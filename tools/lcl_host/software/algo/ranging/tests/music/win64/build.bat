@echo off

@call %~dp0\..\..\..\build2019_env.bat
@call %MSVC_ENV%

%MSVC_CMAKE% -G "Visual Studio 16 2019" -A x64 CMakeLists.txt
REM devenv /build "Release|x64" ranging.sln
MSBuild ranging.sln /t:Rebuild /p:Configuration=Release /m
