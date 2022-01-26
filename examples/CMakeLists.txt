add_executable(fwoophttp_example 
    httpserver.m.cpp
)
target_include_directories(fwoophttp_example PUBLIC .)
target_link_libraries(fwoophttp_example fwoophttp pthread)
