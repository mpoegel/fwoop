add_executable(fig
    fig.m.cpp
)
target_include_directories(fig PUBLIC .)
target_link_libraries(fig fwoophttp fwoopbasis pthread)
install(TARGETS fig DESTINATION usr/local/bin)
