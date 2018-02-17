cmake_minimum_required(VERSION 3.9)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-g)
    add_compile_options(-O2)

    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Wpedantic)
endif()

# Check for Boost
set(Boost_USE_STATIC_LIBS   ON)

find_package(
        Boost "1.66.0"
	REQUIRED COMPONENTS
	date_time
	program_options
	filesystem
	system
	unit_test_framework
)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Check for OpenSSL
set(OPENSSL_USE_STATIC_LIBS ON)

find_package(OpenSSL REQUIRED)

include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})

# Check support for interprocedural optimization (aka. LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)

project(bpistats)

include_directories(${PROJECT_SOURCE_DIR}/src)

if (MSVC)
	add_compile_options("/FIglobal.hpp")
endif()

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

set(BPISTATS_SOURCES "${SOURCE_DIR}/bpistats.cpp")

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.hpp")
list(REMOVE_ITEM SOURCES "${BPISTATS_SOURCES}")

string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_SOURCE_DIR)
set(PLATFORM_SOURCE_DIR "${SOURCE_DIR}/${PLATFORM_SOURCE_DIR}")
if (EXISTS "${PLATFORM_SOURCE_DIR}")
	file(GLOB PLATFORM_SOURCES "${PLATFORM_SOURCE_DIR}/*.cpp")

	foreach(_platform_file ${PLATFORM_SOURCES})
		string(FIND "${_platform_file}" "/" _last_path_separator REVERSE)
		math(EXPR _last_path_separator "${_last_path_separator} + 1")
		string(SUBSTRING "${_platform_file}" ${_last_path_separator} -1 _file)

		list(REMOVE_ITEM SOURCES "${SOURCE_DIR}/${_file}")
	endforeach()

	list(APPEND SOURCES "${PLATFORM_SOURCES}")
endif()

add_library(bpicore STATIC ${SOURCES})

# XXX al - automatic library detection is broken, expect boost from https://dl.bintray.com/boostorg/release/1.66.0/binaries/
target_link_libraries(bpicore
	${Boost_LIBRARIES}
	${OPENSSL_LIBRARIES}
)
if (UNIX AND NOT APPLE)
	target_link_libraries(bpicore
		dl
		z
		stdc++fs
		pthread
		rt
		)
elseif (MSVC)
	target_link_libraries(bpicore
		legacy_stdio_definitions.lib # for missing __imp__* symbols
		crypt32.lib # for missing __imp_Cert* symbols
		)
endif()

# Main executable
add_executable(bpistats "${BPISTATS_SOURCES}")

target_link_libraries(bpistats bpicore)
if (ipo_supported)
	set_property(TARGET bpicore PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_property(TARGET bpistats PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (UNIX AND NOT APPLE)
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

target_link_libraries(bpitests bpicore)

if (ipo_supported)
	set_property(TARGET bpitests PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

enable_testing()
