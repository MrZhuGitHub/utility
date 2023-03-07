if [ ! -d "./build" ]; then
    mkdir build
fi
cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=Debug -DTEST=YES
make -j4
make install