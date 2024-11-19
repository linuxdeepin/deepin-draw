# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.16)

# 设置基础库名称为 "deepinDrawBase"，并将其存储在缓存中
SET(BASE_LIB "deepinDrawBase" CACHE STRING INTERNAL FORCE)

# 设置cmake参数
set(CMAKE_CXX_STANDARD 17)  # 设置C++标准为C++14
set(CMAKE_INCLUDE_CURRENT_DIR ON)  # 包含当前目录
set(CMAKE_AUTOMOC ON)  # 自动处理moc文件
set(CMAKE_AUTORCC ON)  # 自动处理rcc文件
set(CMAKE_AUTOUIC ON)  # 自动处理uic文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wl,--as-needed -fPIE")  # 设置C++编译标志，启用调试信息和警告
set(QT_MINIMUM_VERSION "6.0.0")  # 定义Qt的最低版本为6.0.0
set(CMAKE_EXE_LINKER_FLAGS "-pie")  # 设置可执行文件的链接标志

# 设置编译器/链接器标志
if(DEEPINDRAW_TEST)
    message(DEEPINDRAW_TESTDEEPINDRAW_TESTDEEPINDRAW_TESTDEEPINDRAW_TEST)
    # 如果定义了DEEPINDRAW_TEST，表示需要创建可以获取覆盖信息的库
    # mips不支持-fsanitize
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
        set(CMAKE_CXX_FLAGS "-g -O1 -fprofile-arcs -ftest-coverage")
        set(CMAKE_C_FLAGS "-g -O1 -fprofile-arcs -ftest-coverage")
        set(CMAKE_Ｌ_FLAGS "-g -O1 -fprofile-arcs -ftest-coverage -lgcov")
    else()
        message(DEEPINDRAW_TESTDEEPINDRAW_TESTDEEPINDRAW_TESTDEEPINDRAW_TEST1)
        set(CMAKE_CXX_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer -g -O1 -fprofile-arcs -ftest-coverage")
        set(CMAKE_C_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer -g -O1 -fprofile-arcs -ftest-coverage")
        set(CMAKE_Ｌ_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer -g -O1 -fprofile-arcs -ftest-coverage -lgcov")
    endif()
else()
    # 如果没有定义DEEPINDRAW_TEST，表示只需要一个不包含-fsanitize或-ftest-coverage的清晰库
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        # 在DEBUG模式下不进行优化以方便调试
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    else()
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
    endif()

    # 有时我们也想测试deepin-draw的内存信息
    if(DEEPINDRAW_SANITIZERS)
        set(CMAKE_CXX_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
        set(CMAKE_C_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
    endif()
endif()

# 设置Qt模块
if(QT_VERSION_MAJOR EQUAL 6)
    set(QtModule Core Gui Widgets DBus PrintSupport Svg SvgWidgets Concurrent LinguistTools)
    find_package(Qt6 REQUIRED COMPONENTS ${QtModule})
else()
    set(QtModule Core Gui Widgets DBus PrintSupport Svg Concurrent LinguistTools)
    find_package(Qt5 REQUIRED COMPONENTS ${QtModule})
endif()

#选择DTK版本
if(QT_VERSION_MAJOR EQUAL "6")  # 如果Qt版本为6
    set(BUILD_WITH_QT6 ON)  # 启用Qt6构建
    set(DTK_VERSION_MAJOR 6)  # 设置DTK版本为6
else()
    set(DTK_VERSION_MAJOR "")  # 否则不设置DTK版本
endif()
message("Testing Using dtk version: ${DTK_VERSION_MAJOR}")  # 输出使用的DTK版本

# 配置文件，将config.h.in转换为config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)

# 定义源码目录
set(SRCDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/drawshape
    ${CMAKE_CURRENT_SOURCE_DIR}/drawshape/drawTools
    ${CMAKE_CURRENT_SOURCE_DIR}/drawshape/drawItems
    ${CMAKE_CURRENT_SOURCE_DIR}/drawshape/drawItems/bzItems
    ${CMAKE_CURRENT_SOURCE_DIR}/frame
    ${CMAKE_CURRENT_SOURCE_DIR}/service
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/dialog
    ${CMAKE_CURRENT_SOURCE_DIR}/frame/AttributesWidgets
    ${CMAKE_CURRENT_SOURCE_DIR}/frame/AttributesWidgets/private
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    )
SET(BASE_LIB_INCLUDEDIRS "${SRCDIRS}" CACHE STRING INTERNAL FORCE)

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

# 查找翻译文件
FILE(GLOB targetTsFiles ../translations/*.ts)

# 创建翻译文件，将源文件与ts文件关联生成qm文件
if(QT_VERSION_MAJOR EQUAL 6)
    qt6_create_translation(tragetQmFiles ${allSource} ${allHeaders} ${targetTsFiles})
else()
    qt5_create_translation(tragetQmFiles ${allSource} ${allHeaders} ${targetTsFiles})
endif()

# 设置不删除生成的文件夹内容文件（设置此项是为了翻译后的文件不被删除）
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

# 添加资源文件
if(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_resources(RESOURCE_ADDED ${allQrcs})
else()
        qt5_add_resources(RESOURCE_ADDED ${allQrcs})
endif()

# 根据条件选择创建静态库或共享库
if(DEEPINDRAW_LINK_DRAWBASELIB_STATIC)
    add_library(${BASE_LIB} STATIC  ${allSource} ${allHeaders} ${RESOURCE_ADDED} ${tragetQmFiles})
else()
    add_library(${BASE_LIB} SHARED  ${allSource} ${allHeaders} ${RESOURCE_ADDED} ${tragetQmFiles})
endif()

# 设置目标属性
set_target_properties(${BASE_LIB}
  PROPERTIES
  VERSION 0.0
  SOVERSION 0
  PUBLIC_HEADER "${allHeaders}")

# 在相应的构建目录中查找包含文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

# 添加第三方库 DTK
# 使用第三方库需要用到的一个包
find_package(PkgConfig REQUIRED)

# 检查第三方库(这里检查了名字为dtkwidget的库和名字为dtkgui的库)，然后取名3RMODULES
pkg_check_modules(3RMODULES REQUIRED
        dtk6widget dtk6gui dtk6core
        )

# 添加调试信息
message(STATUS "DTK Widget Include Dirs: ${3RMODULES_INCLUDE_DIRS}")
message(STATUS "DTK Libraries: ${3RMODULES_LIBRARIES}")
message(STATUS "DTK Library Dirs: ${3RMODULES_LIBRARY_DIRS}")

# 添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)
target_include_directories(${BASE_LIB} PUBLIC ${3RMODULES_INCLUDE_DIRS})
target_link_libraries(${BASE_LIB} ${3RMODULES_LIBRARIES} ${CMAKE_DL_LIBS})

# 将工程与Qt模块链接起来
# if(QT_MAJOR_VERSION EQUAL 6)
#     foreach(module ${QtModule})
#         target_link_libraries(${BASE_LIB} Qt6::${module})
#     endforeach()
#     message(STATUS "QtModule contents qt6: ${QtModule}")
# else()
#     qt5_use_modules(${BASE_LIB} ${QtModule})
#     message(STATUS "QtModule contents qt5: ${QtModule}")
# endif()
# 设置链接库
set(LINK_LIBS
    Qt6::Core
    Qt6::DBus
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Svg
    Qt6::Concurrent
    Dtk6::Widget
    Dtk6::Gui
    Dtk6::Core
)

# 替换原来的 if-else 链接逻辑
target_link_libraries(${BASE_LIB} ${LINK_LIBS})

# 安装脚本
set(DbusDir ${CMAKE_INSTALL_DATADIR}/dbus-1/services/)
set(DesktopDir ${CMAKE_INSTALL_DATADIR}/applications/)
set(ManIconDir ${CMAKE_INSTALL_DATADIR}/icons/deepin/apps/scalable)
set(AppIconDir ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
set(MimeFileDir ${CMAKE_INSTALL_DATADIR}/mime/packages)
set(MimeTypeDir ${CMAKE_INSTALL_DATADIR}/application)
set(HelpFileDir ${CMAKE_INSTALL_DATADIR}/deepin-manual/manual-assets/application)

# 如果不是静态链接库，安装库文件
if(NOT DEEPINDRAW_LINK_DRAWBASELIB_STATIC)
    install(TARGETS ${BASE_LIB}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif()

# 安装翻译文件
install(FILES ${tragetQmFiles} DESTINATION ${TranslationDir})

# 安装手册图标
install(FILES ./logo/deepin-draw.svg DESTINATION ${ManIconDir})

# 安装应用图标
install(FILES ./logo/deepin-draw.svg DESTINATION ${AppIconDir})

# 添加子目录
add_subdirectory(deepin-draw)
