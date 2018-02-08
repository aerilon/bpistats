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
        Boost "1.66.0"
        REQUIRED COMPONENTS
        program_options
        regex
        system
        thread
        unit_test_framework
)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

project(bpistats)

file(GLOB SOURCES src/*.cpp src/*.hpp)

add_executable(bpistats ${SOURCES})

if (UNIX AND NOT APPLE)
	target_link_libraries(bpistats
		pthread
		rt)
endif()

add_subdirectory(tests)

enable_testing()
