# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (script2000)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_executable(script2000
	src/main.cpp
	src/grammar.cpp
	src/parse_table.cpp
	src/parser.cpp
)

target_link_libraries(script2000
)


