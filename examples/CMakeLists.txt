add_executable(fwoophttpserver
    httpserver.m.cpp
)
target_include_directories(fwoophttpserver PUBLIC .)
target_link_libraries(fwoophttpserver fwoophttp pthread)

add_executable(fwoophttpclient
    httpclient.m.cpp
)
target_include_directories(fwoophttpclient PUBLIC .)
target_link_libraries(fwoophttpclient fwoophttp pthread)
