cmake_minimum_required(VERSION 3.0.0)
project(stone-lang VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(include D:\\Programming\\C++\\libs\\VS\\boost\\boost_1_81_0)
link_directories(libs D:\\Programming\\C++\\libs\\VS\\boost\\boost_1_81_0\\stage\\lib)

add_executable(stone-lang src/main.cpp src/parser.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
