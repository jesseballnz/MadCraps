cmake_minimum_required(VERSION 3.10)
project(MadCrapsRules)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_library(madcraps_rules STATIC
    src/RulesEngine.cpp
)

target_include_directories(madcraps_rules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(simulator tools/simulator.cpp)
target_link_libraries(simulator madcraps_rules)

add_executable(large_simulator tools/large_simulator.cpp)
target_link_libraries(large_simulator madcraps_rules)

add_executable(rules_tests tests/rules_tests.cpp)
target_link_libraries(rules_tests madcraps_rules)

add_executable(rules_tests_expanded tests/rules_tests_expanded.cpp)
target_link_libraries(rules_tests_expanded madcraps_rules)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME rules_tests COMMAND rules_tests)
    add_test(NAME rules_tests_expanded COMMAND rules_tests_expanded)
endif()

install(TARGETS madcraps_rules
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(TARGETS simulator large_simulator
    RUNTIME DESTINATION bin
)

install(FILES
    src/RulesEngine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table_config_vegas.json
    DESTINATION share/madcraps-rules
)

set(CPACK_PACKAGE_NAME "madcraps-rules")
set(CPACK_PACKAGE_VENDOR "MadCraps")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_CONTACT "opensource@madcraps.local")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MadCraps rules engine prototype and tools")
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "MadCraps Rules")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "MadCraps")
endif()

include(CPack)
