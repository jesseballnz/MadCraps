cmake_minimum_required(VERSION 3.10)
project(MadCrapsRules)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(madcraps_rules STATIC
    src/RulesEngine.cpp
)

target_include_directories(madcraps_rules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(simulator tools/simulator.cpp)
target_link_libraries(simulator madcraps_rules)

add_executable(rules_tests tests/rules_tests.cpp)
target_link_libraries(rules_tests madcraps_rules)
