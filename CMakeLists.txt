cmake_minimum_required(VERSION 3.12)
project(mul_utl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MUL_UTL_BUILD_EXAMPLES "Build documentation examples" OFF)
option(MUL_UTL_BUILD_TESTS "Build documentation tests" OFF)

add_library(Mul_Utl INTERFACE)
target_include_directories(Mul_Utl
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

add_library(JED::mul_utl ALIAS Mul_Utl)

if(MUL_UTL_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(MUL_UTL_BUILD_TESTS)
	add_subdirectory(tests)
endif()