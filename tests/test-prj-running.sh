QTEST_FUNCTION_TIMEOUT=500000
export QT_QPA_PLATFORM='offscreen'
cd ../
[ -d build-ut ] && rm -fr build-ut || mkdir build-ut
[ -d build ] && rm -fr build 
mkdir -p build
cp tests/collection-coverage.sh build/
cd build/
cmake ../tests/
#make test -j8
make -j8

lcov --directory ./CMakeFiles/deepin-draw-test.dir/ --zerocounters
./deepin-draw-test
#第一次会因为未知原因失败
./collection-coverage.sh
sleep 5
#第一次因为未知原因失败,执行第二次进行收集
./collection-coverage.sh

cd ../
mkdir -p ./build-ut
#拷贝覆盖率到指定的路径
cp -r ./build/coverageResult/report ./build-ut
#拷贝内存泄漏检测信息到指定的路径
cp ./build/asan_deepin-draw.log* ./build-ut/asan_deepin-draw.log
cd build-ut
mv ./report ./html
cd html
mv ./index.html ./cov_deepin-draw.html
mkdir ../report
cp ../../build/report/report_deepin-draw.xml ../report/
chmod 751 cov_deepin-draw.html

