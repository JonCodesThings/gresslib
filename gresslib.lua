workspace "gresslib_workspace"
  configurations { "Debug", "Release" }
  platforms { "x86", "x86_64" }
  location "build"

project "gresslib"
  kind "StaticLib"
  language "C"
  includedirs { "." }
  filter "system:Windows"
    files { "include/gresslib/*.h", "src/win32/*.c", "src/internal/*.c", "src/WGL/*.c" }
  filter "system:Linux"
    files { "include/gresslib/*.h", "src/x11/*.c", "src/internal/*.c", "src/glx/*.c" }