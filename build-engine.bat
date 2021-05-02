@echo off

mkdir "build/int"
pushd "build/int"

::TIMER TAKEN FROM  https://stackoverflow.com/questions/673523/how-do-i-measure-execution-time-of-a-command-on-the-windows-command-line
set start=%time%

cl -Zi  /EHsc /c ..\..\chaosengine\chaos.cpp ..\..\chaosengine\src\chaospch.cpp /I ..\..\chaosengine\vendor\spdlog\include /I ..\..\chaosengine\src /I ..\..\chaosengine\vendor /I ..\..\chaosengine\vendor\imgui /I ..\..\chaosengine\vendor\glfw\include /I ../../chaosengine/vendor/vulkan/include /I ../../chaosengine/vendor/glm/glm /std:c++17 /D "CHAOS_PLATFORM_WINDOWS" /D "CHAOS_RELEASE" /MT

popd

mkdir "build/bin"
pushd "build/bin"

lib ..\int\chaos.obj ..\int\chaospch.obj
MOVE ..\int\chaos.lib ..\bin

popd

set end=%time%
set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%

:: Mission accomplished
set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
echo build took %hours%:%mins%:%secs%.%ms% (%totalsecs%.%ms%s total)

