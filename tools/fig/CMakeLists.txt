add_executable(fig
    fig.m.cpp
)
target_include_directories(fig PUBLIC .)
target_link_libraries(fig fwoophttp fwoopbasis pthread)
set_property(TARGET fig PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_compile_options(fig PRIVATE "-Wall")

install(TARGETS fig DESTINATION bin)
