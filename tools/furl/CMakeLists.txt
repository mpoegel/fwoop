add_executable(furl
    furl.m.cpp
)
target_include_directories(furl PUBLIC .)
target_link_libraries(furl fwoophttp fwoopbasis pthread)
install(TARGETS furl DESTINATION usr/local/bin)
