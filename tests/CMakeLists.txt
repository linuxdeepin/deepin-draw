# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)

# 设置cmake参数
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 添加gocv覆盖率文件的输出
set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")

# 设置Qt模块
set(QtModule Core Gui Widgets Network DBus Sql PrintSupport Svg Concurrent LinguistTools)

# 设置工程名字
project(deepin-draw-test)

# 查找Qt相关的库链接
find_package(Qt5 REQUIRED ${QtModule})

# 包含源码文件夹
include_directories(../src)
include_directories(../src/drawshape)
include_directories(../src/frame)
include_directories(../src/qtsingleapplication)
include_directories(../src/service)
include_directories(../src/utils)
include_directories(../src/widgets)
include_directories(../src/widgets/dialog)

include_directories(googletest)
include_directories(googletest/googlemock/include)
include_directories(googletest/googletest/include)

add_subdirectory(googletest)

# 定义 source 文件
FILE (GLOB allSource
    "../src/application.cpp"
    "../src/drawshape/*.cpp"  "../src/drawshape/*.c"
    "../src/frame/*.cpp"  "../src/frame/*.c"
    "../src/qtsingleapplication/*.cpp"  "../src/qtsingleapplication/*.c"
    "../src/service/*.cpp"  "../src/service/*.c"
    "../src/utils/*.cpp"  "../src/utils/*.c"
    "../src/widgets/*.cpp"  "..src/widgets/*.c"
    "../src/widgets/dialog/*.cpp"  "../src/widgets/dialog/*.c"
    )
FILE (GLOB allTestSource
    "./*.cpp" "./*.c")

# 定义 header 文件
FILE (GLOB allHeaders
    "../src/*.hpp"  "../src/application.h"
    "../src/drawshape/*.hpp"  "../src/drawshape/*.h"
    "../src/frame/*.hpp"  "../src/frame/*.h"
    "../src/qtsingleapplication/*.hpp"  "../src/qtsingleapplication/*.h"
    "../src/service/*.hpp"  "../src/service/*.h"
    "../src/utils/*.hpp"  "../src/utils/*.h"
    "../src/widgets/*.hpp"  "../src/widgets/*.h"
    "../src/widgets/dialog/*.hpp"  "../src/widgets/dialog/*.h"
    )

# 定义资源文件
set(AllQRC
    ../src/icon/icons.qrc
    ../src/icons/theme-icons.qrc
    ../src/drawshape/cursorIcons.qrc
    ../src/drawshape/images.qrc
    ../src/frame/frameRes.qrc
    ../src/widgets/widgetsRes.qrc)

# 设置不删除生成的文件夹内容文件（设置此项是为了翻译后的文件不被删除）
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

# 定义翻译文件
set(TargetTsFiles
    translations/deepin-draw_zh_CN.ts
    translations/deepin-draw_en_US.ts
    translations/deepin-draw.ts
    )

# 添加需要安装的模块
add_executable(${PROJECT_NAME} ${allHeaders} ${allSource} ${allTestSource} ${AllQRC}
    )
## Create the gcov target. Run coverage tests with 'make gcov'
#add_custom_target(gcov
#    COMMAND mkdir -p coverage
#    COMMAND ${CMAKE_MAKE_PROGRAM} test
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    )
#add_custom_command(TARGET gcov
#    COMMAND echo "=================== GCOV ===================="
#    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/*.cpp -o ${OBJECT_DIR}
#    | grep -A 5 "Adder.cpp" > CoverageSummary.tmp
#    COMMAND cat CoverageSummary.tmp
#    COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
#    )
#add_dependencies(gcov ${PROJECT_NAME})
## Make sure to clean up the coverage folder
#set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)

## Create the gcov-clean target. This cleans the build as well as generated
## .gcda and .gcno files.
#add_custom_target(scrub
#COMMAND ${CMAKE_MAKE_PROGRAM} clean
#COMMAND rm -f ${OBJECT_DIR}/*.gcno
#COMMAND rm -f ${OBJECT_DIR}/*.gcda
#WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#)

#------------------------------ 添加第三方库  DTK -------------------------------
# 使用第三方库需要用到的一个包
find_package(PkgConfig REQUIRED)

# 检查第三方库(这里检查了名字为dtkwidget的库和名字为dtkgui的库)，然后取名3rd_lib
pkg_check_modules(3rd_lib REQUIRED
        dtkwidget dtkgui
        )
# 添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)
target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS})

# 将第三方库链接进来(注意 *_LIBRARIES)
target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES} )
#------------------------------添加第三方库end-------------------------------------

target_link_libraries(${PROJECT_NAME} gmock gmock_main gtest gtest_main)

# 将工程与Qt模块链接起来
qt5_use_modules(${PROJECT_NAME} ${QtModule})

# ------------------------------- install script -------------------------------
#execute_process(COMMAND bash ci/lcov.sh)
#set(PREFIX /usr)
#set(BinDir ${PREFIX}/bin)
#set(TranslationDir ${PREFIX}/share/deepin-draw/translations/)
#set(DbusDir ${PREFIX}/share/dbus-1/services/)
#set(DesktopDir ${PREFIX}/share/applications/)
#set(FormatFileDir ${PREFIX}/share/mime/packages/)
#set(ManIconDir ${PREFIX}/share/icons/hicolor/scalable/apps)
#set(AppIconDir ${PREFIX}/share/icons/hicolor/scalable/apps)
#set(MimeFileDir ${PREFIX}/share/mime/packages)

# install Targets:
#   * <prefix>/lib/libxxx.so
#   * header location after install: <prefix>/include/xxx.h
#   * headers can be included by C++ code `#include <xxx.h>`
#install(TARGETS ${PROJECT_NAME}
#        EXPORT ${PROJECT_NAME}Targets
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib
#        RUNTIME DESTINATION /usr/bin
#        INCLUDES DESTINATION allHeaders)

# install Translation files:
#install(FILES ${LangSrcs} DESTINATION ${TranslationDir})

# install DBus file:
#install(FILES com.deepin.Draw.service DESTINATION ${DbusDir})

# install Desktop icon
#install(FILES deepin-draw.desktop DESTINATION ${DesktopDir})

# install FormatFile ---> unused
#install(FILES src/service/x-ddf.xml DESTINATION ${FormatFileDir})

# install Manual icon
#install(FILES src/logo/deepin-draw.svg DESTINATION ${ManIconDir})

# install App icon
#install(FILES src/logo/deepin-draw.svg DESTINATION ${AppIconDir})

# install MimeFile
#install(FILES mimetype/deepin-draw.xml DESTINATION ${MimeFileDir})
