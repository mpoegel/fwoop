add_executable(fwoophttpserver
    httpserver.m.cpp
)
target_include_directories(fwoophttpserver PUBLIC .)
target_link_libraries(fwoophttpserver fwoophttp fwoopobserve pthread)

add_executable(fwoophttpclient
    httpclient.m.cpp
)
target_include_directories(fwoophttpclient PUBLIC .)
target_link_libraries(fwoophttpclient fwoophttp pthread)

add_executable(fwooptlsclient
    tlsclient.m.cpp
)
target_include_directories(fwooptlsclient PUBLIC .)
target_link_libraries(fwooptlsclient fwoopbasis fwoopcrypto pthread)
