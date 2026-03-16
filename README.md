# WoS Patches

This is mostly a compendium of various patches that I make to allow users to continue to use WoS in some form or another.

## Current Patches

- **SRNet.dll Patch** — Patches SRNet.dll by changing the domain URL to the new domain URL for synreal.ini fetching. Also creates a backup copy of the original DLL.

---

## SRNet.dll Patcher

Two versions are available depending on your preference.

---

### Go Version (Recommended)

A single standalone `.exe` with no dependencies or additional files required.

**Usage**

Run `SRNetPatch.exe` and follow the prompt, or drag and drop `SRNet.dll` onto the exe. You can also run it from the command line:

```
SRNetPatch.exe "C:\path\to\SRNet.dll"
```

**Building from source**

Don't trust random exes from strangers on the internet — source is provided so you can build it yourself. You'll need [Go](https://go.dev/dl/) installed.

```
go build -o SRNetPatch.exe SRNetPatch.go
```

---

### C++ Version (GUI)

A graphical version with a file picker and progress bar. Requires three DLL files to run (`libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`) which are included in the release alongside the exe.

**Usage**

GUI: Run `SRNetPatcher.exe`, select `SRNet.dll`, click Patch.

CLI: `SRNetPatcher.exe "C:\path\to\SRNet.dll"`

**Building from source**

Don't trust random exes from strangers on the internet — source is provided so you can build it yourself.

*MSYS2 / MinGW* — [https://www.msys2.org](https://www.msys2.org)

Install the toolchain if you haven't:
```
pacman -S mingw-w64-x86_64-toolchain
```

Then from the MSYS2 MinGW 64-bit shell:
```
g++ -o SRNetPatcher.exe SRNetPatcher.cpp -luser32 -lgdi32 -lcomdlg32 -lcomctl32 -mwindows
```

*Visual Studio* — From a Developer Command Prompt:
```
cl SRNetPatcher.cpp /EHsc /link user32.lib gdi32.lib comdlg32.lib comctl32.lib /SUBSYSTEM:WINDOWS /OUT:SRNetPatcher.exe
```

Or create an Empty C++ Project, add `SRNetPatcher.cpp`, and build. Set subsystem to Windows (not Console) under Project Properties → Linker → System → SubSystem.
