add_library(fwoophttp STATIC
    fwoop_httpdataframe.cpp
    fwoop_httpserver.cpp
    fwoop_httpframe.cpp
    fwoop_httpgoawayframe.cpp
    fwoop_httpheader.cpp
    fwoop_httpheadersframe.cpp
    fwoop_httphpacker.cpp
    fwoop_httpsettingsframe.cpp
    fwoop_httpstream.cpp
    fwoop_httpwindowupdateframe.cpp
)
target_include_directories(fwoophttp PUBLIC .)
target_link_libraries(fwoophttp fwoopbasis)

install(TARGETS fwoophttp DESTINATION lib)
install(FILES
    fwoop_httpdataframe.h
    fwoop_httpserver.h
    fwoop_httpframe.h
    fwoop_httpgoawayframe.h
    fwoop_httpheader.h
    fwoop_httpheadersframe.h
    fwoop_httphpacker.h
    fwoop_httpsettingsframe.h
    fwoop_httpstream.h
    fwoop_httpwindowupdateframe.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoophttp_test
    fwoop_httpheadersframe.g.cpp
    fwoop_httpserver.g.cpp
    fwoop_httphpacker.g.cpp
    fwoop_httpsettingsframe.g.cpp
)
target_include_directories(fwoophttp_test PUBLIC .)
target_link_libraries(fwoophttp_test fwoophttp fwoopbasis gtest gtest_main pthread)
gtest_add_tests(TARGET fwoophttp_test)
