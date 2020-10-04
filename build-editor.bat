@echo off

mkdir "build/cmd/int"
pushd "build/cmd/int"

::TIMER TAKEN FROM  https://stackoverflow.com/questions/673523/how-do-i-measure-execution-time-of-a-command-on-the-windows-command-line
set start=%time%

pushd "build/cmd/int"
cl -Zi -c ../../../ControlEditor/control.cpp /I ../../../chaosengine/src /I ../../../chaosengine/vendor /I ../../../chaosengine/vendor/spdlog/include /I ../../../chaosengine/vendor/imgui /I ../../../chaosengine/vendor/glfw/include /I ../../../controleditor/src /D "CHAOS_PLATFORM_WINDOWS" /D "CHAOS_RELEASE" /EHsc  /MT /std:c++17

popd

mkdir "build/cmd/bin"
pushd "build/cmd/bin"

link ..\int\control.obj Chaos.lib ..\..\..\ChaosEngine\vendor\GLFW\bin\Release-windows-x86_64\GLFW\glfw.lib ..\..\..\ChaosEngine\vendor\Vulkan\Lib\vulkan-1.lib ..\..\..\ChaosEngine\vendor\ImGUI\bin\Release-windows-x86_64\imgui\imgui.lib gdi32.lib shell32.lib msvcrtd.lib /NODEFAULTLIB:LIBCMTD /NODEFAULTLIB:LIBCMT
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

pushd "build/cmd/bin"

start  control.exe
popd
