mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target colex
sudo cmake --install .
rm -rf build

