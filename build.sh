rm -rf build/
mkdir build
cd build
cmake ..
make
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
