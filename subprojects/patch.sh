#!/bin/bash

set -e

cd ..

patch -p0 < patches/chaiscript.patch
patch -p0 < patches/imgui-text-editor.patch