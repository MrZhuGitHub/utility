if [ ! -d "./build" ]; then
    mkdir build
fi
cd build
make clean
cmake ..
make -j4
make install