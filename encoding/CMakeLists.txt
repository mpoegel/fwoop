add_library(fwoopencoding STATIC
    fwoop_json.cpp
    fwoop_protobuf.cpp
    fwoop_svg.cpp
    fwoop_template.cpp
)

target_include_directories(fwoopencoding PUBLIC .)

install(TARGETS fwoopencoding DESTINATION lib)
target_link_libraries(fwoopencoding fwoopbasis)
install(FILES
    fwoop_json.h
    fwoop_protobuf.h
    fwoop_svg.h
    fwoop_template.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopencoding_test
    fwoop_json.g.cpp
    fwoop_protobuf.g.cpp
    fwoop_svg.g.cpp
    fwoop_template.g.cpp
)
target_compile_options(fwoopencoding_test PRIVATE -fsanitize=address)
target_link_options(fwoopencoding_test PRIVATE -fsanitize=address)
target_include_directories(fwoopencoding_test PUBLIC .)
target_link_libraries(fwoopencoding_test fwoopencoding fwoopbasis gtest gtest_main pthread)

gtest_add_tests(TARGET fwoopencoding_test)
