# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)

# 设置cmake参数
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置工程名字
project(deepin-draw)

# 是否开启单元测试编译
option(CREATTEST "Compile and generate test program" OFF)

# 是否开启内存泄露检测
option(SANITIZERS "do sanitizers for code" OFF)

set(TOP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 加速编译优化参数
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
    SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
    SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
    SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
else()
    SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
    SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3  -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
    SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
endif()

if(SANITIZERS)
    set(CMAKE_CXX_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
endif()

# 设置Qt模块
set(QtModule Core Gui Widgets DBus PrintSupport Svg Concurrent LinguistTools)

# 查找Qt相关的库链接
find_package(Qt5 REQUIRED ${QtModule})

set(SRCDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drawshape
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drawshape/drawTools
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drawshape/drawItems
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drawshape/drawItems/bzItems
    ${CMAKE_CURRENT_SOURCE_DIR}/src/frame
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/dialog
    ${CMAKE_CURRENT_SOURCE_DIR}/src/frame/AttributesWidgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/frame/AttributesWidgets/private
    ${CMAKE_CURRENT_SOURCE_DIR}/src/icon
    ${CMAKE_CURRENT_SOURCE_DIR}/src/icons
    )

# 包含源码文件夹并获取其下所有源代码文件
foreach(item IN LISTS SRCDIRS)
    include_directories(${item})
    list(APPEND SRC_CPPS    ${item}/*.cpp)
    list(APPEND SRC_HEADERS ${item}/*.h)
    list(APPEND SRC_QRCS    ${item}/*.qrc)
endforeach()
FILE (GLOB allSource  ${SRC_CPPS})
FILE (GLOB allHeaders ${SRC_HEADERS})
FILE (GLOB allQrcs    ${SRC_QRCS})

FILE(GLOB targetTsFiles ./translations/*.ts)

# 创建翻译文件(将源文件${allSource} ${allHeaders}关联上${targetTsFiles}的ts文件 然后生成${tragetQmFiles}的qm文件)
qt5_create_translation(tragetQmFiles ${allSource} ${allHeaders} ${targetTsFiles})

# 设置不删除生成的文件夹内容文件（设置此项是为了翻译后的文件不被删除）
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

add_executable(${PROJECT_NAME} ${allSource} ${allHeaders} ${allQrcs} ${tragetQmFiles})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)

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
target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES})

#------------------------------添加第三方库end-------------------------------------

# 将工程与Qt模块链接起来
qt5_use_modules(${PROJECT_NAME} ${QtModule})


# ------------------------------- install script -------------------------------

set(PREFIX /usr)
set(BinDir ${PREFIX}/bin)
set(TranslationDir ${PREFIX}/share/deepin-draw/translations/)
set(DbusDir ${PREFIX}/share/dbus-1/services/)
set(DesktopDir ${PREFIX}/share/applications/)
set(ManIconDir ${PREFIX}/share/icons/hicolor/scalable/apps)
set(AppIconDir ${PREFIX}/share/icons/hicolor/scalable/apps)
set(MimeFileDir ${PREFIX}/share/mime/packages)

# install Targets:
#   * <prefix>/lib/libxxx.so
#   * header location after install: <prefix>/include/xxx.h
#   * headers can be included by C++ code `#include <xxx.h>`
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION /usr/bin
        INCLUDES DESTINATION allHeaders)

# install Translation files:
install(FILES ${tragetQmFiles} DESTINATION ${TranslationDir})

# install DBus file:
install(FILES ./com.deepin.Draw.service DESTINATION ${DbusDir})

# install Desktop icon
install(FILES ./deepin-draw.desktop DESTINATION ${DesktopDir})

# install Manual icon
install(FILES ./src/logo/deepin-draw.svg DESTINATION ${ManIconDir})

# install App icon
install(FILES ./src/logo/deepin-draw.svg DESTINATION ${AppIconDir})

# install MimeFile
install(FILES ./mimetype/deepin-draw.xml DESTINATION ${MimeFileDir})

# 添加测试子工程
if(CREATTEST)
add_subdirectory(tests)
endif()

