# NESEmulator

Inspiré par la série de vidéos de `javidx9` "NES Emulator From Scratch" https://www.youtube.com/playlist?list=PLrOv9FMX8xJHqMvSGB_9G9nZZ_4IgteYf

# Build

A partir de la racine du projet :


```bash
./build.sh
```

ou

```bash
cd build
cmake ..
cd ..
cmake --build build/ -- -j 8
./FNES_emulator.exe
```

# Dépendances

```
sfml
nlohmann-json
```

# Installation

## Installation sous Windows avec MSYS2

- Installer MSYS2
- Aller dans la console de MSYS2
- Installer les outils de build gcc et les dépendances :
```bash
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S --needed mingw-w64-x86_64-sfml
pacman -S --needed mingw-w64-x86_64-nlohmann-json
```
