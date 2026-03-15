This is basically just a repo for various patches and such for WoS, assuming I make more.

Current patches:
* SRNet.dll Patch - Patches SRNet.dll by changing the domain URL -> new domain URL for
  synreal.ini fetching. Also creates a backup copy of the original DLL.

---

Don't trust random exes from strangers on the internet, source is provided so you can
build it yourself.

Building from source:

MSYS2 / MinGW (https://www.msys2.org)
Install the toolchain if you haven't:
    pacman -S mingw-w64-x86_64-toolchain

Then from the MSYS2 MinGW 64-bit shell:
    g++ -o SRNetPatcher.exe SRNetPatcher.cpp -luser32 -lgdi32 -lcomdlg32 -lcomctl32 -mwindows

Visual Studio
From a Developer Command Prompt:
    cl SRNetPatcher.cpp /EHsc /link user32.lib gdi32.lib comdlg32.lib comctl32.lib /SUBSYSTEM:WINDOWS /OUT:SRNetPatcher.exe

Or create an Empty C++ Project, add SRNetPatcher.cpp, and build. Set subsystem to Windows
(not Console) under Project Properties -> Linker -> System -> SubSystem.

---

Usage:
GUI: Run SRNetPatcher.exe, select SRNet.dll, click Patch.
CLI: SRNetPatcher.exe "C:\path\to\SRNet.dll"
