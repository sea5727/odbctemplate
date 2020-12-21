cmake_minimum_required(VERSION 3.0.0)
project(odbc-tutorial-1 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
include(CTest)
enable_testing()

# link_directories(/usr/local/lib64 /usr/local/lib/mariadb)
# link_libraries(odbc mariadb)
link_directories(/usr/local/lib64 /usr/lib64 )
link_libraries(odbc)

add_executable(test0 test0.cpp)
# add_executable(test1 test1_select_where.cpp)
# add_executable(test2 test2_select_all.cpp)
# add_executable(test3 test3_update.cpp)
# add_executable(test4 test4_update.cpp)
# add_executable(test5 test5_autocommit_on_off.cpp)
# add_executable(test6 test6_connection_pool.cpp)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
