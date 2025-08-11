project(FileToClipboard)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)


set(PROJECT_CPP_SOURCES
    "app/cpp/src/main.cpp"
    "app/cpp/src/X11ClipboardManager.cpp"
    "app/cpp/src/WaylandClipboardManager.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_CPP_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/app/cpp/include" ${X11_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})