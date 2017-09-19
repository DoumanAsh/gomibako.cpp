###########################
# Main exe
###########################
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB main_SRC "main.cpp")

add_subdirectory("config/")
add_subdirectory("http/")
add_subdirectory("utils/")

add_executable(gomibako ${main_SRC})
target_link_libraries(gomibako ${Boost_LIBRARIES} ${HTTP_LIBS} ${UTILS_LIBS} ${CONFIG_LIBS})
target_include_directories(gomibako PUBLIC ${Boost_INCLUDE_DIRS} ${3PP_INCLUDES})
set_target_properties(
  gomibako PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)
