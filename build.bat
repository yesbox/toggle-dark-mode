@echo off
setlocal

set "MSYS2_PATH=C:\msys64"
set "UCRT64_BIN=%MSYS2_PATH%\ucrt64\bin"
set "PATH=%UCRT64_BIN%;%PATH%"

gcc toggle_dark_mode.c -o toggle_dark_mode.exe ^
  -std=c23 -Werror -Wall -Wextra -Wpedantic ^
  -nostdlib -Wl,--entry=Entry ^
  -Os -s -flto -Wl,--gc-sections -Wl,--strip-all ^
  -mwindows -lkernel32 -luser32 -ladvapi32 -lshell32
