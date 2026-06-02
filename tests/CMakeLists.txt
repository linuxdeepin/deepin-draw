# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.16)

# 设置cmake参数（与主工程保持一致，使用 C++17）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置工程名字
project(deepin-draw-test)

# 测试代码需要使用主工程设置的 Qt / DTK 版本
if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(Qt6 QUIET)
    if(Qt6_FOUND)
        set(QT_VERSION_MAJOR 6)
        set(DTK_VERSION_MAJOR 6)
    else()
        find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Test)
        set(QT_VERSION_MAJOR 5)
        set(DTK_VERSION_MAJOR "")
    endif()
endif()
message(STATUS "Tests will be built against Qt${QT_VERSION_MAJOR} / DTK${DTK_VERSION_MAJOR}")

option(DEEPINDRAW_TEST_OFFSCREENT "test in off screen." ON)
if(DEEPINDRAW_TEST_OFFSCREENT)
    add_definitions(-DTEST_OFFSCREENT)
endif()

option(DEEPINDRAW_TEST_DELETE_ITEM "test delete items fuctions" ON)
if(DEEPINDRAW_TEST_DELETE_ITEM)
    add_definitions(-DTEST_DELETE_ITEM)
endif()

option(DEEPINDRAW_TEST_BLUR_ITEM "test blur fuctions" ON)
if(DEEPINDRAW_TEST_BLUR_ITEM)
    add_definitions(-DTEST_BLUR_ITEM)
endif()

option(DEEPINDRAW_TEST_RECT_ITEM "test rectangle item fuctions" ON)
if(DEEPINDRAW_TEST_RECT_ITEM)
    add_definitions(-DTEST_RECT_ITEM)
endif()

option(DEEPINDRAW_TEST_ELLIPSE_ITEM "test ellipse item fuctions" ON)
if(DEEPINDRAW_TEST_ELLIPSE_ITEM)
    add_definitions(-DTEST_ELLIPSE_ITEM)
endif()

option(DEEPINDRAW_TEST_START_ITEM "test star item fuctions" ON)
if(DEEPINDRAW_TEST_START_ITEM)
    add_definitions(-DTEST_START_ITEM)
endif()

option(DEEPINDRAW_TEST_PICTURE_ITEM "test picture item fuctions" ON)
if(DEEPINDRAW_TEST_PICTURE_ITEM)
    add_definitions(-DTEST_PICTURE_ITEM)
endif()

option(DEEPINDRAW_TEST_TRIANGLE_ITEM "test triangle item fuctions" ON)
if(DEEPINDRAW_TEST_TRIANGLE_ITEM)
    add_definitions(-DTEST_TRIANGLE_ITEM)
endif()

option(DEEPINDRAW_TEST_POLYGON_ITEM "test polygon item fuctions" ON)
if(DEEPINDRAW_TEST_POLYGON_ITEM)
    add_definitions(-DTEST_POLYGON_ITEM)
endif()

option(DEEPINDRAW_TEST_LINE_ITEM "test line item fuctions" ON)
if(DEEPINDRAW_TEST_LINE_ITEM)
    add_definitions(-DTEST_LINE_ITEM)
endif()

option(DEEPINDRAW_TEST_PEN_ITEM "test pen item fuctions" ON)
if(DEEPINDRAW_TEST_PEN_ITEM)
    add_definitions(-DTEST_PEN_ITEM)
endif()

option(DEEPINDRAW_TEST_TEXT_ITEM "test text item fuctions" ON)
if(DEEPINDRAW_TEST_TEXT_ITEM)
    add_definitions(-DTEST_TEXT_ITEM)
endif()
option(DEEPINDRAW_TEST_LAYER_ITEM "test layer item fuctions" ON)
if(DEEPINDRAW_TEST_LAYER_ITEM)
    add_definitions(-DTEST_LAYER_ITEM)
endif()

option(DEEPINDRAW_TEST_CUT_ITEM "test cut fuctions" ON)
if(DEEPINDRAW_TEST_CUT_ITEM)
    add_definitions(-DTEST_CUT_ITEM)
endif()

option(DEEPINDRAW_TEST_SCANLE_ITEM "test scanle item fuctions" ON)
if(DEEPINDRAW_TEST_SCANLE_ITEM)
    add_definitions(-DTEST_SCANLE_ITEM)
endif()

option(DEEPINDRAW_TEST_Z_ITEM "test z item fuctions" ON)
if(DEEPINDRAW_TEST_Z_ITEM)
    add_definitions(-DTEST_Z_ITEM)
endif()

option(DEEPINDRAW_TEST_ITEMALIGNMENT_ITEM "test item alignment fuctions" ON)
if(DEEPINDRAW_TEST_ITEMALIGNMENT_ITEM)
    add_definitions(-DTEST_ITEMALIGNMENT_ITEM)
endif()

option(DEEPINDRAW_TEST_FUNCTION_ITEM "test more other fuctions" ON)
if(DEEPINDRAW_TEST_FUNCTION_ITEM)
    add_definitions(-DTEST_FUNCTION_ITEM)
endif()

option(DEEPINDRAW_TEST_DIALOG_ITEM "test dialog fuctions" ON)
if(DEEPINDRAW_TEST_DIALOG_ITEM)
    add_definitions(-DTEST_DIALOG)
endif()

# 设置Qt模块（按 Qt 主版本选择对应组件名）
if(QT_VERSION_MAJOR EQUAL 6)
    set(QtModule Core Gui Widgets DBus PrintSupport Svg SvgWidgets Concurrent Test)
    find_package(Qt6 REQUIRED COMPONENTS ${QtModule})
else()
    set(QtModule Core Gui Widgets DBus PrintSupport Svg Concurrent Test)
    find_package(Qt5 REQUIRED COMPONENTS ${QtModule})
endif()

#注意mips不支持-fsanitize
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
    set(CMAKE_CXX_FLAGS "-g -O1 -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS    "-lgcov")
else()
   set(CMAKE_CXX_FLAGS "-g -O1 -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS    "-fsanitize=undefined,address,leak -lgcov")

    # notice code that we should output memery use info.
    add_definitions(-DENABLE_FSANITIZE)
endif()

# 定义测试用例文件及测试资源文件
set(AllTestFileDirs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/service
    ${CMAKE_CURRENT_SOURCE_DIR}/testItems
    ${CMAKE_CURRENT_SOURCE_DIR}/testUIs
    ${CMAKE_CURRENT_SOURCE_DIR}/testResource
    )
foreach(item IN LISTS AllTestFileDirs)
    include_directories(${item})
    list(APPEND allTestSourceFiles ${item}/*.cpp)
    list(APPEND allTestSourceFiles ${item}/*.c)
    list(APPEND allTestSourceFiles ${item}/*.h)
    list(APPEND allTestQRCFiles    ${item}/*.qrc)
endforeach()
FILE(GLOB allTestSource ${allTestSourceFiles})
FILE(GLOB allTestQRC ${allTestQRCFiles})

add_executable(${PROJECT_NAME} ${allTestQRC} ${allTestSource})

target_include_directories(${PROJECT_NAME} PUBLIC ${BASE_LIB_INCLUDEDIRS})
target_link_libraries(${PROJECT_NAME} gmock gmock_main gtest gtest_main pthread ${CMAKE_DL_LIBS} ${BASE_LIB})

#------------------------------ 添加第三方库  DTK -------------------------------

# 使用第三方库需要用到的一个包
find_package(PkgConfig REQUIRED)

# 检查第三方库(注意 Qt5 下名为 dtkwidget，Qt6 下名为 dtk6widget)
pkg_check_modules(3RMODULES REQUIRED
        dtk${DTK_VERSION_MAJOR}widget
        dtk${DTK_VERSION_MAJOR}gui
        dtk${DTK_VERSION_MAJOR}core
        )

# 添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)
target_include_directories(${PROJECT_NAME} PUBLIC ${3RMODULES_INCLUDE_DIRS})

# 将第三方库链接进来(注意 *_LIBRARIES)
target_link_libraries(${PROJECT_NAME} ${3RMODULES_LIBRARIES})

#------------------------------添加第三方库end-------------------------------------

# 将工程与 Qt 模块链接起来（兼容 Qt5 / Qt6）
foreach(module IN LISTS QtModule)
    target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::${module})
endforeach()
