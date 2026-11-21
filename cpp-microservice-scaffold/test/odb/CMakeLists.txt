
cmake_minimum_required(VERSION 3.16)

project(odb_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 定义测试程序
add_executable(odb_test
    odb_test.cc
    student-odb.cxx
    ../../source/odb.cc
)

# 2. 添加头文件搜索路径
target_include_directories(odb_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../source
)

# 3. 临时解决 MySQL 开发头文件兼容问题
target_compile_options(odb_test PRIVATE
    -include mysql/plugin_auth_common.h
)

# 4. 链接 ODB 运行时库
target_link_libraries(odb_test PRIVATE
    -l:libodb-mysql-2.5.0-b.27.so
    -l:libodb-2.5.0-b.27.so
)
