if (USE_OPENSSL)
add_library(fwoopcrypto STATIC
    fwoop_open_sha.cpp
    fwoop_open_tls.cpp
)
else()
add_library(fwoopcrypto STATIC
    fwoop_sha.cpp
    fwoop_tls.cpp
)
endif()

target_include_directories(fwoopcrypto PUBLIC .)

if(USE_OPENSSL)
    message("using OpenSSL for crypto")
    target_link_libraries(fwoopcrypto ssl)
    add_compile_definitions(USE_OPENSSL)
endif()

install(TARGETS fwoopcrypto DESTINATION lib)
install(FILES
    fwoop_sha.h
    fwoop_tls.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopcrypto_test 
    fwoop_sha.g.cpp
    fwoop_tls.g.cpp
)
target_include_directories(fwoopcrypto_test PUBLIC .)
target_link_libraries(fwoopcrypto_test fwoopcrypto fwoopbasis gtest gtest_main pthread)

if(USE_OPENSSL)
    target_link_libraries(fwoopcrypto_test ssl)
    add_compile_definitions(USE_OPENSSL)
endif()

gtest_add_tests(TARGET fwoopcrypto_test)
