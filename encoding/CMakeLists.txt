add_library(fwoopencoding STATIC
    fwoop_template.cpp
)

target_include_directories(fwoopencoding PUBLIC .)

install(TARGETS fwoopencoding DESTINATION lib)
target_link_libraries(fwoopencoding fwoopbasis)
install(FILES
    fwoop_template.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopencoding_test 
    fwoop_template.g.cpp
)
target_include_directories(fwoopencoding_test PUBLIC .)
target_link_libraries(fwoopencoding_test fwoopencoding fwoopbasis gtest gtest_main pthread)

gtest_add_tests(TARGET fwoopencoding_test)
