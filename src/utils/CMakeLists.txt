set(UTILS_LIBS "utils_format" PARENT_SCOPE)

add_library(utils_format STATIC format.cpp)
target_link_libraries(utils_format ${Boost_LIBRARIES})
target_include_directories(utils_format PUBLIC ${Boost_INCLUDE_DIRS})
