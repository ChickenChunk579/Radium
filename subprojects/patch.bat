@echo off
setlocal
set PATCH_DIR=patches

REM Exit on error
set ERRORLEVEL=

cd ..

REM Apply patch to ChaiScript
echo Applying patch: %PATCH_DIR%\chaiscript.patch
patch -p0 < %PATCH_DIR%\chaiscript.patch
if errorlevel 1 (
    echo Failed to apply chaiscript.patch
    exit /b 1
)

REM Apply patch to ImGuiTextEditor
echo Applying patch: %PATCH_DIR%\imgui-text-editor.patch
patch -p0 < %PATCH_DIR%\imgui-text-editor.patch
if errorlevel 1 (
    echo Failed to apply imgui-text-editor.patch
    exit /b 1
)

echo.
echo All patches applied successfully.
endlocal
pause
