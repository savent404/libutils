cmake_minimum_required(VERSION 3.10)
project(libutils)

set(CMAKE_CXX_STANDARD 14)

option(USE_TEST "compile unit test" OFF)

set(c_inc c/inc)
aux_source_directory(c/src c_src)
add_library(utils_c ${c_src})
target_include_directories(utils_c PUBLIC ${c_inc})

if (USE_TEST)
    include(FetchContent)
    FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    aux_source_directory(test/src ut_src)
    set(ut_inc test/inc)
    add_executable(unit_test ${ut_src})
    target_link_libraries(unit_test PUBLIC utils_c GTest::gtest_main)
    target_include_directories(unit_test PUBLIC ${ut_inc})
endif (USE_TEST)