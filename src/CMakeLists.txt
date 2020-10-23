# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)

# 设置cmake参数
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(DOTEST)
    set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")
else()
# 加速编译优化参数，和单元测试在一起会链接失败
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
        SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
        SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
        SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
    else()
        SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
        SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3  -Wl,-O1 -Wl,--as-needed -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
        SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
    endif()
endif()

# 设置工程名字
project(deepin-draw-bin-lib)

# 设置Qt模块
set(QtModule Core Gui Widgets DBus PrintSupport Svg Concurrent LinguistTools)

# 查找Qt相关的库链接
find_package(Qt5 REQUIRED ${QtModule})

# 包含源码文件夹
include_directories(../)
include_directories(drawshape)
include_directories(drawshape/drawTools)
include_directories(drawshape/drawItems)
include_directories(drawshape/drawItems/bzItems)
include_directories(frame)
include_directories(service)
include_directories(utils)
include_directories(widgets)
include_directories(widgets/dialog)
include_directories(frame/AttributesWidgets)
include_directories(frame/AttributesWidgets/private)

# 定义 source 文件
FILE (GLOB allSource
    "./application.cpp"
    "./drawshape/*.cpp"  "./drawshape/*.c"
    "./drawshape/drawTools/*.cpp"  "./drawshape/drawTools/*.c"
    "./drawshape/drawItems/*.cpp"  "./drawshape/drawItems/*.c"
    "./drawshape/drawItems/bzItems/*.cpp"  "./drawshape/drawItems/bzItems/*.c"
    "./frame/*.cpp"  "./frame/*.c"
    "./frame/AttributesWidgets/*.cpp"  "./frame/AttributesWidgets/*.c"
    "./frame/AttributesWidgets/private/*.cpp"  "./frame/AttributesWidgets/private/*.c"
    "./service/*.cpp"  "./service/*.c"
    "./utils/*.cpp"  "./utils/*.c"
    "./widgets/*.cpp"  "./widgets/*.c"
    "./widgets/dialog/*.cpp"  "./widgets/dialog/*.c"
    )

# 定义 header 文件
FILE (GLOB allHeaders
    "./application.h"
    "./drawshape/*.hpp"  "./drawshape/*.h"
    "./drawshape/drawTools/*.hpp"  "./drawshape/drawTools/*.h"
    "./drawshape/drawItems/*.hpp"  "./drawshape/drawItems/*.h"
    "./drawshape/drawItems/bzItems/*.hpp"  "./drawshape/drawItems/bzItems/*.h"
    "./frame/*.hpp"  "./frame/*.h"
    "./frame/AttributesWidgets/*.hpp"  "./frame/AttributesWidgets/*.h"
    "./frame/AttributesWidgets/private/*.hpp"  "./frame/AttributesWidgets/private/*.h"
    "./service/*.hpp"  "./service/*.h"
    "./utils/*.hpp"  "./utils/*.h"
    "./widgets/*.hpp"  "./widgets/*.h"
    "./widgets/dialog/*.hpp"  "./widgets/dialog/*.h"
    )

# 定义资源文件
#set(AllQRC
#    ../src/icon/icons.qrc
#    ../src/icons/theme-icons.qrc
#    ../src/drawshape/cursorIcons.qrc
#    ../src/drawshape/images.qrc
#    ../src/frame/frameRes.qrc
#    ../src/widgets/widgetsRes.qrc)

file(GLOB OtherTranslationFiles ../translations/*.ts)
set(TargetTsFiles
    ${OtherTranslationFiles}
    )

# 创建翻译文件
qt5_create_translation(LangSrcs ${allSource} ${allHeaders} ${TargetTsFiles})

## 添加翻译
#qt5_add_translation(${TargetTsFiles})

# 设置不删除生成的文件夹内容文件（设置此项是为了翻译后的文件不被删除）
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

add_library(${PROJECT_NAME} STATIC ${allSource} ${allHeaders} ${LangSrcs})

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
set(TranslationDir ${PREFIX}/share/deepin-draw/translations/)

# install Translation files:
install(FILES ${LangSrcs} DESTINATION ${TranslationDir})


