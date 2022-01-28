add_library(fwoophttp STATIC
    fwoop_httpserver.cpp
    fwoop_httpframe.cpp
)
target_include_directories(fwoophttp PUBLIC .)
target_link_libraries(fwoophttp fwoopbasis)

install(TARGETS fwoophttp DESTINATION lib)
install(FILES
    fwoop_httpserver.h
    fwoop_httpframe.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoophttp_test 
    fwoop_httpserver.g.cpp
)
target_include_directories(fwoophttp_test PUBLIC .)
target_link_libraries(fwoophttp_test fwoophttp fwoopbasis gtest gtest_main pthread)
gtest_add_tests(TARGET fwoophttp_test)
