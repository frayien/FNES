(
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cd ..
cmake --build build --config Debug
.\Debug\FNES_emulator.exe
)