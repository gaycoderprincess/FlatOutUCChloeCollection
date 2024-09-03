# FlatOut: Ultimate Carnage Chloe Collection

Companion plugin for my Ultimate Carnage modpack, [Chloe's Collection](https://drive.google.com/file/d/1lDQxvRza0brcR6gADX4yp3_5iqgxwCmr/view?usp=sharing).

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process.

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common) to a folder next to this one, so it can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x86-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib32`

You should be able to build the project now in CLion.
