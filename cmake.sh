mkdir -p build
cp -r config build/
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
