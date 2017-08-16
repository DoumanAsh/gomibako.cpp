###########################
# Main exe
###########################
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB main_SRC "main.cpp")
file(GLOB http_SRC "http/*.cpp")
file(GLOB config_SRC "config/*.cpp")
file(GLOB utils_SRC "utils/*.cpp")

add_executable(gomibako ${main_SRC} ${http_SRC} ${config_SRC} ${utils_SRC})
target_link_libraries(gomibako ${Boost_LIBRARIES})
target_include_directories(gomibako PUBLIC ${Boost_INCLUDE_DIRS} ${3PP_INCLUDES})
