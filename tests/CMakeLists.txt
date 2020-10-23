# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)

# 设置cmake参数
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置工程名字
project(deepin-draw-test)

# 添加gocv覆盖率文件的输出
set(CMAKE_CXX_FLAGS           "-O1 -g -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS             "-O1 -g -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS    "-lgcov")

# 设置Qt模块
if(NOT QtModule)
    set(QtModule Core Gui Widgets DBus PrintSupport Svg Concurrent LinguistTools Test)
else()
    list(APPEND QtModule Test)
endif()

# 查找Qt相关的库链接
find_package(Qt5 REQUIRED ${QtModule})

if(NOT SRCDIRS)
    #定义是独立的cmakelists标记，以表示当前独立打开的工程
    set(INDEPENDENTCMAKELISTS 1)

    message("--it's independent CMake project for deepin-draw test!!")

    # 包含源码文件夹并获取其下所有源代码文件
    set(SRCDIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/drawshape
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/drawshape/drawTools
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/drawshape/drawItems
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/drawshape/drawItems/bzItems
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/frame
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/service
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/widgets/dialog
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/frame/AttributesWidgets
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/frame/AttributesWidgets/private
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/icon
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/icons
        )
    foreach(item IN LISTS SRCDIRS)
        include_directories(${item})
        list(APPEND SRC_CPPS    ${item}/*.cpp)
        list(APPEND SRC_HEADERS ${item}/*.h)
        list(APPEND SRC_QRCS    ${item}/*.qrc)
    endforeach()

    FILE (GLOB allHeaders ${SRC_HEADERS})
    FILE (GLOB allSource  ${SRC_CPPS})
    FILE (GLOB allQrcs    ${SRC_QRCS})
endif()

#去掉主程序的入口main文件(避免两个main函数入口冲突)
if(INDEPENDENTCMAKELISTS)
    list(REMOVE_ITEM allSource ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp)

    #打印要编译的文件
    foreach(item IN LISTS allHeaders)
        message("Will compile .h   file: ${item}")
    endforeach()

    foreach(item IN LISTS allSource)
        message("Will compile .cpp file: ${item}")
    endforeach()
else()
    list(REMOVE_ITEM allSource ${TOP_PROJECT_DIR}/src/main.cpp)
endif()

# 设置测试程序的包含目录
include_directories(googletest)
include_directories(googletest/googlemock/include)
include_directories(googletest/googletest/include)
add_subdirectory(./googletest)

# 定义测试用例文件
FILE (GLOB allTestSource
    "./*.cpp" "./*.c"
    "./service/*.cpp" "./service/*.c"
    "./testItems/*.h" "./testItems/*.cpp" "./testItems/*.c"
    "./testUIs/*.cpp" "./testUIs/*.c"
    )

# 定义测试资源文件
set(AllTestQRC
    ./testResource/testResource.qrc)

add_executable(${PROJECT_NAME} ${allSource} ${allHeaders} ${allQrcs}
    ${AllTestQRC} ${AllQRC} ${allTestSource})

target_link_libraries(${PROJECT_NAME} gmock gmock_main gtest gtest_main)

# 添加 QTest 测试
add_test(${PROJECT_NAME} COMMAND ${PROJECT_NAME})

set(PROJECT_SRC_NAME deepin-draw)

if(INDEPENDENTCMAKELISTS)
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    include_directories(${PROJECT_BINARY_DIR})
    include_directories(${PROJECT_SOURCE_DIR})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)
    set(LCOV_SRC_PATH_DIR ./CMakeFiles/${PROJECT_NAME}.dir/)
    set(LCOV_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    set(LCOV_SRC_PATH_DIR ./CMakeFiles/${PROJECT_NAME}.dir/)
    set(LCOV_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endif()


#------------------------------ 创建'make test'指令---------------------------------------
add_custom_target(test
    COMMAND mkdir -p coverageResult
    WORKING_DIRECTORY ${LCOV_WORKING_DIRECTORY})

add_custom_command(TARGET test
    COMMAND echo " =================== CREAT LCOV REPROT BEGIN ==================== "

    #1.清理原先的gcov信息
    COMMAND lcov --directory ${LCOV_SRC_PATH_DIR} --zerocounters
    COMMAND lcov --directory ${LCOV_SRC_PATH_DIR} --zerocounters
    COMMAND ./${PROJECT_NAME}

    #2.收集gcov信息到.info文件中
    COMMAND lcov --directory ${LCOV_SRC_PATH_DIR} --capture --output-file ./coverageResult/${PROJECT_NAME}_Coverage.info

    #3.过滤一些我们不感兴趣的文件的覆盖率信息
    COMMAND echo " =================== do filter begin ==================== "
    COMMAND lcov --remove ./coverageResult/${PROJECT_NAME}_Coverage.info
    '*/${PROJECT_SRC_NAME}_autogen/*' '*/${PROJECT_NAME}_autogen/*' '*/usr/include/*' '*/tests/*' '*/googletest/*'
    -o ./coverageResult/${PROJECT_NAME}_Coverage.info
    COMMAND echo " =================== do filter end ==================== "

    #3.将.info信息生成报告到reprot文件夹中
    COMMAND genhtml -o ./coverageResult/report ./coverageResult/${PROJECT_NAME}_Coverage.info

    COMMAND echo " =================== CREAT LCOV REPROT END ==================== "

    COMMAND echo " Coverage files have been output to ${LCOV_WORKING_DIRECTORY}/coverageResult/report "

    WORKING_DIRECTORY ${LCOV_WORKING_DIRECTORY}
    )

#'make test'命令依赖与我们的测试程序
add_dependencies(test ${PROJECT_NAME})

#------------------------------ 添加第三方库  DTK -------------------------------

if(INDEPENDENTCMAKELISTS)
    # 使用第三方库需要用到的一个包
    find_package(PkgConfig REQUIRED)

    # 检查第三方库(这里检查了名字为dtkwidget的库和名字为dtkgui的库)，然后取名3rd_lib
    pkg_check_modules(3rd_lib REQUIRED
            dtkwidget dtkgui
            )
endif()

# 添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)
target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS})

# 将第三方库链接进来(注意 *_LIBRARIES)
target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES})

#------------------------------添加第三方库end-------------------------------------

# 将工程与Qt模块链接起来
qt5_use_modules(${PROJECT_NAME} ${QtModule})
