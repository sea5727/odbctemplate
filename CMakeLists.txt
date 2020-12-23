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


add_executable(selectall_mysql_bindcol.EXE selectall_mysql_bindcol.cpp)
add_executable(selectall_mysql_direct.EXE selectall_mysql_direct.cpp)
add_executable(select_mysql_noprepare.EXE select_mysql_noprepare.cpp)
add_executable(select_mysql_prepare.EXE select_mysql_prepare.cpp)
add_executable(selectall_mysql_noprepare.EXE selectall_mysql_noprepare.cpp)
add_executable(selectall_mysql_prepare.EXE selectall_mysql_prepare.cpp)
add_executable(INSERTER2.EXE insert_mysql.cpp)

# add_executable(main2 main2.cpp)
# add_executable(mysql0 test0_one_call.cpp)
# add_executable(mysql1 test0_multi_call.cpp)
# add_executable(INSERTER.EXE insert.cpp)
# add_executable(TRUNCATER.EXE truncate.cpp)
# add_executable(test1 test1_select_where.cpp)
# add_executable(test2 test2_select_all.cpp)
# add_executable(test3 test3_update.cpp)
# add_executable(test4 test4_connection.cpp)
# add_executable(test5 test5_bindcol.cpp)
# add_executable(test6 test6_connection_pool.cpp)
# add_executable(functionchaintest functionchaintest.cpp)


# add_executable(generic templatetest.cpp)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
