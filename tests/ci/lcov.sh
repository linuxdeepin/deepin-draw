#!/bin/sh
echo "start..."
cd ..
cd ..
cd build-tests-unknown-Debug/
./deepin-draw-test
cd CMakeFiles/deepin-draw-test.dir/home/jixianglong/Desktop/project/deepin-draw/src
#lcov --directory . --zerocounters     
lcov --directory . --capture --output-file app.info
genhtml -o ../../../../../../../../results app.info
