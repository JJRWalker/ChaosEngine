@echo off
if not exist ".\spv\" mkdir ".\spv"
for /f %%f in ('dir /b .\glsl') do glslc .\glsl\%%f -o .\spv\%%f.spv