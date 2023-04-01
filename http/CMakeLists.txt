add_library(fwoophttp STATIC
    fwoop_dnsquery.cpp
    fwoop_httpclient.cpp
    fwoop_httpdataframe.cpp
    fwoop_httpframe.cpp
    fwoop_httpgoawayframe.cpp
    fwoop_httpheader.cpp
    fwoop_httpheadersframe.cpp
    fwoop_httphpacker.cpp
    fwoop_httprequest.cpp
    fwoop_httpresponse.cpp
    fwoop_httpserver.cpp
    fwoop_httpserverevent.cpp
    fwoop_httpsettingsframe.cpp
    fwoop_httpstream.cpp
    fwoop_httpversion.cpp
    fwoop_httpwindowupdateframe.cpp
)
target_include_directories(fwoophttp PUBLIC .)
target_link_libraries(fwoophttp fwoopbasis)

install(TARGETS fwoophttp DESTINATION lib)
install(FILES
    fwoop_dnsquery.h
    fwoop_httpclient.h
    fwoop_httpdataframe.h
    fwoop_httpframe.h
    fwoop_httpgoawayframe.h
    fwoop_httpheader.h
    fwoop_httpheadersframe.h
    fwoop_httphpacker.h
    fwoop_httprequest.h
    fwoop_httpresponse.h
    fwoop_httpserver.h
    fwoop_httpserverevent.h
    fwoop_httpsettingsframe.h
    fwoop_httpstream.h
    fwoop_httpversion.h
    fwoop_httpwindowupdateframe.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoophttp_test
    fwoop_dnsquery.g.cpp
    fwoop_httpheadersframe.g.cpp
    fwoop_httphpacker.g.cpp
    fwoop_httprequest.g.cpp
    fwoop_httpresponse.g.cpp
    fwoop_httpserver.g.cpp
    fwoop_httpserverevent.g.cpp
    fwoop_httpsettingsframe.g.cpp
)
target_include_directories(fwoophttp_test PUBLIC .)
target_link_libraries(fwoophttp_test fwoophttp fwoopbasis gtest gtest_main pthread)
gtest_add_tests(TARGET fwoophttp_test)
