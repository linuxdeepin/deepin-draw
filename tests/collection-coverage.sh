lcov --directory ./CMakeFiles/deepin-draw-test.dir/ --capture --output-file ./coverageResult/deepin-draw-test_Coverage.info
echo \ ===================\ do\ filter\ begin\ ====================\ 
lcov --remove ./coverageResult/deepin-draw-test_Coverage.info '*/deepin-draw_autogen/*' '*/deepin-draw-test_autogen/*' '*/usr/include/*' '*/tests/*' '*/googletest/*' -o ./coverageResult/deepin-draw-test_Coverage.info
echo \ ===================\ do\ filter\ end\ ====================\ 
genhtml -o ./coverageResult/report ./coverageResult/deepin-draw-test_Coverage.info
