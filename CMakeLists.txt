###########################
# Init
##########################
cmake_minimum_required(VERSION 3.6)
set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})
include(LazyUtils)

# Generates compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Gomibako LANGUAGES CXX)

###########################
# Compiler flags
##########################
if (WIN32)
    # We assume that minimum runtime is Vista
    add_definitions(-D_WIN32_WINNT=0x0600)

    # Enforces WinAPI to use unicode version of functions
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

remove_msvc_default_warning_level()
set_msvc_flags_if("/W4 /std:c++latest /source-charset:utf-8")
set_gcc_flags_if("-Wall -Wextra")

###########################
# Linter
##########################
option(CLANG_TIDY "Enable Clang tidy checks" OFF)
if (CLANG_TIDY)
    include(ClangTidy)
endif()

###########################
# Output dir
##########################
set_output_dir("${CMAKE_BINARY_DIR}/dist")

###########################
# Boost
##########################
lazy_find_boost(system)

###########################
# 3pp
##########################
add_subdirectory("3pp/")

###########################
# SRC
##########################
add_subdirectory("src/")

############
# Tests
############
option(UNIT_TEST "Build unit tests" OFF)
if (UNIT_TEST)
    add_subdirectory("test/")
endif()

###########################
# cppcheck
##########################
add_custom_target(
    cppcheck
    COMMAND cppcheck
    --enable=all
    -q
    --force
    -I "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    -I ${3PP_INCLUDES}
    -I ${Boost_INCLUDE_DIRS}
    --suppress=unusedFunction:*:*
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
