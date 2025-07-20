cmake -B build -S . -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make -C build
./build/bin/debug/phisi_app 0 16
