# FlatOut: Ultimate Carnage Chloe Collection

Companion plugin for my Ultimate Carnage modpack, [Chloe's Collection](https://gaycoderprincess.github.io/project/chloe-collection).

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process.

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common) and [nya-common-fouc](https://github.com/gaycoderprincess/nya-common-fouc) to folders next to this one, so they can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x86-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib32`

You should be able to build the project now in CLion.
