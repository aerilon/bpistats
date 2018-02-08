cmake_minimum_required(VERSION 3.10)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-g)
    add_compile_options(-O2)

    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Wpedantic)
endif()

set(Boost_USE_STATIC_LIBS on)
find_package(
        Boost "1.64.0"
)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

project(bpistats)

file(GLOB SOURCES src/*.cpp src/*.hpp)

add_executable(bpistats ${SOURCES})

target_link_libraries(bpistats
	boost_program_options)

if (UNIX AND NOT APPLE)
	target_link_libraries(bpistats
		stdc++fs
		pthread
		rt)
endif()

add_subdirectory(tests)

enable_testing()
