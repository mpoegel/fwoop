add_library(fwoopobserve STATIC
    fwoop_countermetric.cpp
    fwoop_gaugehistogrammetric.cpp
    fwoop_gaugemetric.cpp
    fwoop_metric.cpp
    fwoop_openmetrics.cpp
    fwoop_unit.cpp
)

target_include_directories(fwoopobserve PUBLIC .)

install(TARGETS fwoopobserve DESTINATION lib)
target_link_libraries(fwoopobserve fwoopbasis fwoophttp)
install(FILES
    fwoop_countermetric.h
    fwoop_gaugehistogrammetric.h
    fwoop_gaugemetric.h
    fwoop_metric.h
    fwoop_openmetrics.h
    fwoop_unit.h
    DESTINATION include)

include(GoogleTest)
add_executable(fwoopobserve_test
    fwoop_countermetric.g.cpp
    fwoop_gaugehistogrammetric.g.cpp
    fwoop_gaugemetric.g.cpp
    fwoop_openmetrics.g.cpp
)
target_include_directories(fwoopobserve_test PUBLIC .)
target_link_libraries(fwoopobserve_test fwoopobserve fwoopbasis fwoophttp gtest gtest_main pthread)

gtest_add_tests(TARGET fwoopobserve_test)
