add_library(fwoopcrypto STATIC
    fwoop_sha.cpp
)
target_include_directories(fwoopcrypto PUBLIC .)

install(TARGETS fwoopcrypto DESTINATION lib)
install(FILES
    fwoop_sha.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopcrypto_test 
    fwoop_sha.g.cpp
)
target_include_directories(fwoopcrypto_test PUBLIC .)
target_link_libraries(fwoopcrypto_test fwoopcrypto fwoopbasis gtest gtest_main pthread)
gtest_add_tests(TARGET fwoopcrypto_test)
