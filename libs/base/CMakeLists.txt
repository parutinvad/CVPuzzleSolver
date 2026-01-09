add_library(libbase STATIC
        libbase/configure_working_directory.cpp
        libbase/fast_random.cpp
        libbase/stats.cpp
        libbase/timer.cpp
)

target_include_directories(libbase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_TESTING)
    add_executable(libbase_tests
            libbase/configure_working_directory_tests.cpp
            libbase/fast_random_tests.cpp
            libbase/stats_tests.cpp
            libbase/timer_tests.cpp
    )
    target_link_libraries(libbase_tests PRIVATE libbase GTest::gtest_main)
    add_test(NAME libbase_tests COMMAND libbase_tests)
endif ()
