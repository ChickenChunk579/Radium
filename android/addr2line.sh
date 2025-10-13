$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-addr2line \
  -f \
  -e ./distribution/android/app/intermediates/stripped_native_libs/debug/stripDebugDebugSymbols/out/lib/x86_64/libTestApp.so \
  0x$1
