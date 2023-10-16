add_library(fwoopcrypto STATIC
    fwoop_sha.cpp
    fwoop_securesocket.cpp
    fwoop_tlscredentials.cpp
)

target_include_directories(fwoopcrypto PUBLIC . /usr/local/include/botan-3)
target_link_libraries(fwoopcrypto fwoopbasis botan-3)

install(TARGETS fwoopcrypto DESTINATION lib)
install(FILES
    fwoop_sha.h
    fwoop_securesocket.h
    fwoop_tlscredentials.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopcrypto_test
    fwoop_sha.g.cpp
    fwoop_securesocket.g.cpp
)
target_include_directories(fwoopcrypto_test PUBLIC . /usr/local/include/botan-3)
target_link_libraries(fwoopcrypto_test fwoopcrypto fwoopbasis botan-3 gtest gtest_main pthread)

gtest_add_tests(TARGET fwoopcrypto_test)
