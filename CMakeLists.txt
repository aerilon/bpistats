cmake_minimum_required(VERSION 3.9)

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

set(Boost_USE_STATIC_LIBS   ON)

find_package(
        Boost "1.66.0"
	REQUIRED COMPONENTS
	filesystem
	program_options
	system
	unit_test_framework
)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(CMAKE_EXE_LINKER_FLAGS "-L ${Boost_LIBRARY_DIRS}")

project(bpistats)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(BPISTATS_SOURCES "${PROJECT_SOURCE_DIR}/src/bpistats.cpp")

file(GLOB SOURCES src/*.cpp src/*.hpp)
list(REMOVE_ITEM SOURCES "${BPISTATS_SOURCES}")

add_library(bpicore STATIC ${SOURCES})

# Main executable
add_executable(bpistats "${BPISTATS_SOURCES}")

target_link_libraries(bpistats
	bpicore
	${Boost_LIBRARIES})

if (UNIX AND NOT APPLE)
	target_link_libraries(bpistats
		stdc++fs
		pthread
		rt)

	add_custom_command(
		TARGET bpistats
		POST_BUILD
		COMMAND objcopy --only-keep-debug ${PROJECT_BINARY_DIR}/bpistats ${PROJECT_BINARY_DIR}/bpistats.symbols
		COMMAND strip --strip-debug --strip-unneeded ${PROJECT_BINARY_DIR}/bpistats
		COMMAND objcopy --add-gnu-debuglink ${PROJECT_BINARY_DIR}/bpistats.symbols ${PROJECT_BINARY_DIR}/bpistats
	)
endif()

# Tests
file(GLOB TEST_SOURCES tests/*.cpp tests/*.hpp)

add_executable(bpitests ${TEST_SOURCES})

target_link_libraries(bpitests
	bpicore)

enable_testing()
