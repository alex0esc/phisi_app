cmake -B build\ -S . -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja -C build\
.\build\bin\release\phisi_app.exe