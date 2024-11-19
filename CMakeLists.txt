# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.16.0)

# 设置工程名字
project(deepin-draw)

# 设置CMake参数
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wl,--as-needed -fPIE")
set(QT_MINIMUM_VERSION "6.0.0")
set(CMAKE_EXE_LINKER_FLAGS "-pie")

# 查找Qt库
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Widgets Gui DBus Xml Svg Test)
message("Using Qt version: ${QT_VERSION_MAJOR}")

# 选择DTK版本
if(QT_VERSION_MAJOR EQUAL "6")
    set(BUILD_WITH_QT6 ON)
    set(DTK_VERSION_MAJOR 6)
else()
    set(DTK_VERSION_MAJOR "")
endif()
message("Using dtk version: ${DTK_VERSION_MAJOR}")

# 查找DTK库
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Widget Gui Core)

# 包含目录
include_directories(${DtkWidget_INCLUDE_DIRS} ${DtkGui_INCLUDE_DIRS} ${DtkCore_INCLUDE_DIRS})

# 是否开启单元测试编译
option(DEEPINDRAW_TEST "only compile and generate deepin-draw-test program." OFF)

# 是否开启内存泄露检测
option(DEEPINDRAW_SANITIZERS "add sanitizers flag for code in gcc.it's a gcc function,but not support on mips." OFF)

# 是否启用accessblity(CI自动化测试.if open, we should add accesibility name for qt widgets.)
option(DEEPINDRAW_AC_FUNC_ENABLE "make CI/AT enable.if open, we should add accesibility name for qt widgets." ON)
if(DEEPINDRAW_AC_FUNC_ENABLE)
    add_definitions(-DENABLE_ACCESSIBILITY)
endif()

# 是否启用平板模式(平板适配)
option(DEEPINDRAW_TABLET_ENABLE "do some outlooking changed with code that could make deepin-draw run on tablet system." OFF)
if(DEEPINDRAW_TABLET_ENABLE)
    add_definitions(-DENABLE_TABLETSYSTEM)
endif()

# if want to support plugins, must generate share drawbaselib instead of static)
option(DEEPINDRAW_LINK_DRAWBASELIB_STATIC "static compile deepin-draw or deepin-draw-test." ON)
if(DEEPINDRAW_LINK_DRAWBASELIB_STATIC)
    add_definitions(-DLINK_DRAWBASELIB_STATIC)
endif()

option(DEEPINDRAW_LOAD_TOOL_PLUGINS "if load tool plugins." OFF)
if(DEEPINDRAW_LOAD_TOOL_PLUGINS AND NOT DEEPINDRAW_LINK_DRAWBASELIB_STATIC)
    add_definitions(-DLOAD_TOOL_PLUGINS)
endif()

#define install destination
include(GNUInstallDirs)
set(TranslationDir ${CMAKE_INSTALL_DATADIR}/deepin-draw/translations/)
if(DEEPINDRAW_LOAD_TOOL_PLUGINS AND NOT DEEPINDRAW_LINK_DRAWBASELIB_STATIC)
    set(PluginDir ${CMAKE_INSTALL_LIBDIR}/deepin-draw/plugins/)
    set(PluginTransDir ${TranslationDir}plugins)
endif()

#1. base lib and main executable
add_subdirectory(src)

#2. test executable
if(DEEPINDRAW_TEST)
    add_subdirectory(tests)
endif()

#3.tool plugins(the plugins is for deepin-draw,so lib of deepindrawbase(define in ./src) must not be static)
if(DEEPINDRAW_LOAD_TOOL_PLUGINS AND NOT DEEPINDRAW_LINK_DRAWBASELIB_STATIC)
    message(support plugins----------------)
    add_subdirectory(deepin-draw-plugins)
endif()

# 设置链接库
set(LINK_LIBS
    Qt6::Core
    Qt6::DBus
    Qt6::Widgets
    Qt6::Xml
    Qt6::Svg
    Qt6::Test
    Qt6::SvgWidgets
    Dtk${DTK_VERSION_MAJOR}::Widget
    Dtk${DTK_VERSION_MAJOR}::Gui
    Dtk${DTK_VERSION_MAJOR}::Core
    ${DFrameworkDBus_LIBRARIES}
)

# 安装目标
install(TARGETS deepin-draw DESTINATION ${CMAKE_INSTALL_BINDIR})
file(GLOB QM_FILES "${CMAKE_CURRENT_LIST_DIR}/translations/*.qm")

