add_executable(tests_world TransformTest.cc)
target_link_libraries(tests_world vendor_gmock vendor_gtest_main glove)

add_dwarf_test(tests_world)