###########################
# Main exe
###########################
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB main_SRC "main.cpp")

add_executable(gomibako ${main_SRC})
target_link_libraries(gomibako ${Boost_LIBRARIES})
target_include_directories(gomibako PUBLIC ${Boost_INCLUDE_DIRS})
