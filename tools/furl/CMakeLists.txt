add_executable(furl
    furl.m.cpp
)
target_include_directories(furl PUBLIC .)
target_link_libraries(furl fwoophttp fwoopbasis fwoopencoding fwoopcrypto pthread)
set_property(TARGET furl PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_compile_options(furl PRIVATE "-Wall")

install(TARGETS furl DESTINATION bin)
