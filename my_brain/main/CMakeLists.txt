cmake_minimum_required(VERSION 3.0.0) 
project(MyBrain)


# 添加头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# 链接二进制文件
add_executable(MyBrain 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain.cc 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/parser.cc 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pipeline.cc 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/rules.cc 
    main.cc
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(MyBrain OpenMP::OpenMP_CXX)
endif()

# 放可执行文件的位置，对整个项目所生成的库都生效
make_directory(${CMAKE_BINARY_DIR}/bin) 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

