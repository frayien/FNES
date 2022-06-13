(
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..
cmake --build build --config Release
.\Release\FNES_emulator.exe
)