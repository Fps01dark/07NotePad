# 设置最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称和版本
project(OneNotePad VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "A NotePad")

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 用于自动处理 Qt 的 MOC、UIC 和 RCC 文件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/Qt6/6.8.0/msvc2022_64/lib/cmake/Qt6;" CACHE STRING "Prefix path for CMake packages" FORCE)

# 自动收集 Source 目录中的所有 .cpp 文件
file(GLOB SOURCES "../Source/*.cpp")
list(APPEND SOURCES "../Resources/one_note_pad.qrc")
# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})
# 如果有自定义的头文件路径，可以通过 target_include_directories 添加
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Source)

# 查找 Qt6 包，启用核心、GUI、Widgets 模块
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
# 目标链接外部库
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt6_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
