#!/bin/sh
echo "start..."
cd ../../build-tests-unknown-Debug/CMakeFiles/deepin-draw-test.dir
lcov --directory . --zerocounters  
./../../deepin-draw-test
lcov --directory . --capture --output-file deepin-draw_Coverage.info
genhtml -o ../../results deepin-draw_Coverage.info
