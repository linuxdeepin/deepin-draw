#!/bin/bash
cd ../
cmake . -B build -D CREATTEST=on
cd build
make test -j16
cd ../
mkdir ./build-ut
cp -r ./build/tests/coverageResult/report ./build-ut
cd build-ut
mv ./report ./html
cd html
mv ./index.html ./cov_deepin-draw.html
mkdir ../report
cp ../../build/tests/report/report_deepin-draw.xml ../report/
chmod 751 cov_deepin-draw.html
