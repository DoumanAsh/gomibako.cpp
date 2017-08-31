file(GLOB http_SRC "*.cpp")

set(HTTP_LIBS "http" PARENT_SCOPE)

add_library(http STATIC ${http_SRC})
target_link_libraries(http ${Boost_LIBRARIES})
target_include_directories(http PUBLIC ${Boost_INCLUDE_DIRS} ${3PP_INCLUDES})
