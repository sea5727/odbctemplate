cmake_minimum_required(VERSION 3.0.0)
project(odbc-tutorial-1 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(.)

link_directories(/usr/local/lib64 /usr/lib64 )
link_libraries(odbc pthread)

add_subdirectory(test)
