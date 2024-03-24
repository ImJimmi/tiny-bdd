cmake_minimum_required(VERSION 3.20)
project(tiny-bdd VERSION 1.0.0)

add_library(tiny-bdd INTERFACE)
target_include_directories(tiny-bdd
INTERFACE
    ${CMAKE_SOURCE_DIR}
)

target_compile_features(tiny-bdd
INTERFACE
    cxx_std_17
)

option(TBDD_GENERATE_TEST_RUNNER "Generate the Tiny-BDD test runner?" ON)

if(TBDD_GENERATE_TEST_RUNNER)
    add_executable(tiny-bdd-tests
        test.cpp
    )

    target_link_libraries(tiny-bdd-tests
    PUBLIC
        tiny-bdd
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$" OR CMAKE_CXX_COMPILER_ID STREQUAL "GCC")
        target_compile_options(tiny-bdd-tests
        PUBLIC
            -pedantic
            -Wall
            -Wcast-align
            -Wconversion
            -Wdouble-promotion
            -Wextra
            -Wformat=2
            -Wimplicit-fallthrough
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wpedantic
            -Wshadow
            -Wsign-conversion
            -Wunused
        )
    elseif(CMAKE_CXX_COMPILERID STREQUAL "MSVC")
    endif()

    include(CTest)
    add_test(
        NAME "Test tiny-bdd"
        COMMAND $<TARGET_FILE:tiny-bdd-tests>
    )
endif()
