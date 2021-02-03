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

# 是否启用accessblity(自动化测试)
option(AC_FUNC_ENABLE "make AT enable" ON)
if(AC_FUNC_ENABLE)
    add_definitions(-DENABLE_ACCESSIBILITY)
endif()

# 加速编译优化参数
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
    SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
    SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
endif()


# 设置Qt模块
set(QtModule Core Gui Widgets Network DBus PrintSupport Svg Concurrent LinguistTools)
# 查找Qt相关的库链接
find_package(Qt5 REQUIRED ${QtModule})

# 包含源码文件夹
include_directories(src)
include_directories(src/drawshape)
include_directories(src/drawshape/drawTools)
include_directories(src/drawshape/drawItems)
include_directories(src/drawshape/drawItems/bzItems)
include_directories(src/frame)
include_directories(src/qtsingleapplication)
include_directories(src/service)
include_directories(src/utils)
include_directories(src/widgets)
include_directories(src/widgets/dialog)
include_directories(src/frame/AttributesWidgets)
include_directories(src/frame/AttributesWidgets/private)

# 定义 source 文件
FILE (GLOB allSource
    "./src/application.cpp"
    "./src/drawshape/*.cpp"  "./src/drawshape/*.c"
    "./src/drawshape/drawTools/*.cpp"  "./src/drawshape/drawTools/*.c"
    "./src/drawshape/drawItems/*.cpp"  "./src/drawshape/drawItems/*.c"
    "./src/drawshape/drawItems/bzItems/*.cpp"  "./src/drawshape/drawItems/bzItems/*.c"
    "./src/frame/*.cpp"  "./src/frame/*.c"
    "./src/frame/AttributesWidgets/*.cpp"  "./src/frame/AttributesWidgets/*.c"
    "./src/frame/AttributesWidgets/private/*.cpp"  "./src/frame/AttributesWidgets/private/*.c"
    "./src/qtsingleapplication/*.cpp"  "./src/qtsingleapplication/*.c"
    "./src/service/*.cpp"  "./src/service/*.c"
    "./src/utils/*.cpp"  "./src/utils/*.c"
    "./src/widgets/*.cpp"  "./src/widgets/*.c"
    "./src/widgets/dialog/*.cpp"  "./src/widgets/dialog/*.c"
    )

# 定义 header 文件
FILE (GLOB allHeaders
    "./src/application.h"
    "./src/drawshape/*.hpp"  "./src/drawshape/*.h"
    "./src/drawshape/drawTools/*.hpp"  "./src/drawshape/drawTools/*.h"
    "./src/drawshape/drawItems/*.hpp"  "./src/drawshape/drawItems/*.h"
    "./src/drawshape/drawItems/bzItems/*.hpp"  "./src/drawshape/drawItems/bzItems/*.h"
    "./src/frame/*.hpp"  "./src/frame/*.h"
    "./src/frame/AttributesWidgets/*.hpp"  "./src/frame/AttributesWidgets/*.h"
    "./src/frame/AttributesWidgets/private/*.hpp"  "./src/frame/AttributesWidgets/private/*.h"
    "./src/qtsingleapplication/*.hpp"  "./src/qtsingleapplication/*.h"
    "./src/service/*.hpp"  "./src/service/*.h"
    "./src/utils/*.hpp"  "./src/utils/*.h"
    "./src/widgets/*.hpp"  "./src/widgets/*.h"
    "./src/widgets/dialog/*.hpp"  "./src/widgets/dialog/*.h"
    )

# 定义资源文件
set(AllQRC
    src/icon/icons.qrc
    src/icons/theme-icons.qrc
    src/drawshape/cursorIcons.qrc
    src/drawshape/images.qrc
    src/frame/frameRes.qrc
    src/widgets/widgetsRes.qrc)

# 设置不删除生成的文件夹内容文件（设置此项是为了翻译后的文件不被删除）
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

# 定义翻译文件
file(GLOB OtherTranslationFiles translations/*.ts)
set(TargetTsFiles
    ${OtherTranslationFiles}
    )

# 创建翻译文件
qt5_create_translation(LangSrcs ${allSource} ${allHeaders} ${TargetTsFiles})

# 添加翻译
qt5_add_translation(${TargetTsFiles})

# 添加需要安装的模块
add_executable(${PROJECT_NAME} ${allHeaders} ${allSource} ${AllQRC} ${LangSrcs} "./src/main.cpp")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)

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
#set(FormatFileDir ${PREFIX}/share/mime/packages/)
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
install(FILES ${LangSrcs} DESTINATION ${TranslationDir})

# install DBus file:
install(FILES com.deepin.Draw.service DESTINATION ${DbusDir})

# install Desktop icon
install(FILES deepin-draw.desktop DESTINATION ${DesktopDir})

# install FormatFile ---> unused
#install(FILES src/service/x-ddf.xml DESTINATION ${FormatFileDir})

# install Manual icon
install(FILES src/logo/deepin-draw.svg DESTINATION ${ManIconDir})

# install App icon
install(FILES src/logo/deepin-draw.svg DESTINATION ${AppIconDir})

# install MimeFile
install(FILES mimetype/deepin-draw.xml DESTINATION ${MimeFileDir})


