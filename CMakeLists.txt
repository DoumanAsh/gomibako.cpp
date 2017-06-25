project(Gomibako C CXX)

cmake_minimum_required(VERSION 3.0)

###########################
# Init
##########################
set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})
include(LazyUtils)

# Generates compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

###########################
# Compiler flags
##########################
set_msvc_flags_if("/W4 /EHsc /O2")
set_gcc_flags_if("-Wall -Wextra -O2")
# For MinGW we assume that minimum runtime is Vista
set_gcc_flags_if("-D_WIN32_WINNT=0x0600")

set_msvc_flags_if("/DMG_ENABLE_MQTT=0 /DMG_ENABLE_COAP=0 /DMG_ENABLE_HTTP_WEBSOCKET=0")
set_gcc_flags_if("-DMG_ENABLE_MQTT=0 -DMG_ENABLE_COAP=0 -DMG_ENABLE_HTTP_WEBSOCKET=0")

###########################
# Mongoose
##########################
file(GLOB mongoose_SRC "3pp/mongoose-6.8/*.c")
add_library(mongoose STATIC ${mongoose_SRC})
include_directories("3pp/mongoose-6.8")

###########################
# Gomibako exe
##########################
file(GLOB_RECURSE gomibako_SRC "src/*.cpp")
add_executable(gomibako ${gomibako_SRC})
target_link_libraries(gomibako ${Boost_LIBRARIES} mongoose)

###########################
# cppcheck
##########################
add_custom_target(
    cppcheck
    COMMAND cppcheck
    --enable=all
    --force
    -I "${CMAKE_CURRENT_SOURCE_DIR}/3pp/mongoose-6.8/"
    -I "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    ${gomibako_SRC}
)

############
# Tests
############
option(UNIT_TEST "Build unit tests" OFF)
if (UNIT_TEST)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    include(DownloadProject)
    download_project(
        PROJ googletest
        URL  https://github.com/google/googletest/archive/release-1.8.0.zip
    )

    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

    file(GLOB test_SRC "test/*.cpp")
    add_executable(utest "${test_SRC}")
    target_link_libraries(utest gtest gtest_main lazy_winapi)
endif()
