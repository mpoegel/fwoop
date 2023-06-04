add_library(fwoopbasis STATIC
    fwoop_argparser.cpp
    fwoop_datetime.cpp
    fwoop_filereader.cpp
    fwoop_log.cpp
    fwoop_socketio.cpp
    fwoop_tokenizer.cpp
)
target_include_directories(fwoopbasis PUBLIC .)

install(TARGETS fwoopbasis DESTINATION lib)
install(FILES
    fwoop_argparser.h
    fwoop_datetime.h
    fwoop_filereader.h
    fwoop_log.h
    fwoop_socketio.h
    fwoop_tokenizer.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopbasis_test
    fwoop_argparser.g.cpp
    fwoop_datetime.g.cpp
    fwoop_filereader.g.cpp
    fwoop_log.g.cpp
    fwoop_tokenizer.g.cpp
)
target_include_directories(fwoopbasis_test PUBLIC .)
target_link_libraries(fwoopbasis_test fwoopbasis gtest gtest_main pthread)
gtest_add_tests(TARGET fwoopbasis_test)
