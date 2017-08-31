lazy_find_boost(unit_test_framework)

file(GLOB_RECURSE test_SRC "*.cpp")
add_executable(utest ${test_SRC})
target_link_libraries(utest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} http)
target_include_directories(utest PUBLIC ${Boost_INCLUDE_DIRS} ${3PP_INCLUDES})
