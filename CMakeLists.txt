cmake_minimum_required(VERSION 3.10)
project(FotoPlaner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(FotoPlaner main.cpp)
target_link_libraries(FotoPlaner PRIVATE Qt6::Widgets)