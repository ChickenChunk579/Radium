@echo off
echo Pinned clone script generated on %DATE% %TIME%
echo.

REM Clone and checkout specific commits

git clone --recursive https://github.com/erincatto/box2d "box2d"
cd "box2d"
git checkout cfa9ef1b76256f92ec3e26a1a20521e50dddac19
cd ..

git clone --recursive https://github.com/ChaiScript/ChaiScript "ChaiScript"
cd "ChaiScript"
git checkout 2eb3279c391854c7a005b82ad121802e88b7c171
cd ..

git clone --recursive https://github.com/ocornut/imgui "imgui"
cd "imgui"
git checkout 8f3f4282288b17aee798b6fade6787a8a17843ca
cd ..

git clone --recursive https://github.com/BalazsJako/ImGuiColorTextEdit "ImGuiColorTextEdit"
cd "ImGuiColorTextEdit"
git checkout 0a88824f7de8d0bd11d8419066caa7d3469395c4
cd ..

git clone --recursive https://github.com/aiekick/ImGuiFileDialog "ImGuiFileDialog"
cd "ImGuiFileDialog"
git checkout e76383898501dab0d0fddc23a1ccbfb46105f5cd
cd ..

git clone --recursive https://github.com/lua/lua "lua"
cd "lua"
git checkout 934fdd481ced3a9d4a7aaace4479ce889ab23582
cd ..

git clone --recursive https://github.com/libsdl-org/SDL "SDL"
cd "SDL"
git checkout 39a0fab4d79ccd622512d6028ea3733bd4085455
cd ..

git clone --recursive https://github.com/libsdl-org/SDL_image "SDL_image"
cd "SDL_image"
git checkout 377d5a7df5adcaeb48717baf80f40dc78042512c
cd ..

git clone --recursive https://github.com/libsdl-org/SDL_ttf "SDL_ttf"
cd "SDL_ttf"
git checkout 8f8e77248c1ec1b1b54972e68f44d57ff9851582
cd ..

git clone --recursive https://github.com/eidheim/tiny-process-library "tiny-process-library"
cd "tiny-process-library"
git checkout 5bc47531a97f80ac07092ac3ed56a26139c748b3
cd ..

git clone --recursive https://github.com/wolfpld/tracy "tracy"
cd "tracy"
git checkout 863bcc46357bc117d8d2080845c9c43ad84b9304
cd ..

echo.
echo All repositories cloned and checked out to pinned commits.
pause
