rm -rf build/*
rm -rf bin/*
cd build
cmake ..
make
../bin/solve
