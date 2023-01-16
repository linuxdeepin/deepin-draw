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

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${3RMODULES_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${3RMODULES_LIBRARIES} ${CMAKE_DL_LIBS} ${BASE_LIB})

# 将工程与Qt模块链接起来
qt5_use_modules(${PROJECT_NAME} ${QtModule})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION /usr/lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION /usr/bin
        INCLUDES DESTINATION allHeaders)

# install DBus file:
install(FILES ../../com.deepin.Draw.service DESTINATION ${DbusDir})

# install Desktop icon
install(FILES ../../deepin-draw.desktop DESTINATION ${DesktopDir})

# install MimeFile
install(FILES ../../mimetype/deepin-draw.xml DESTINATION ${MimeFileDir})

# install Help File
install(DIRECTORY ../../assets/deepin-draw DESTINATION ${HelpFileDir})

