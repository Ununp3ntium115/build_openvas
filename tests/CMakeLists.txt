# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.10)

message("-- Configuring AI Service Tests...")

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ai-engine)
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${JSON_GLIB_INCLUDE_DIRS})

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLIB_CFLAGS_OTHER}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JSON_GLIB_CFLAGS_OTHER}")

# Test executables
add_executable(test_ai_service
    test_ai_service.c
)

add_executable(test_integration
    test_integration.c
)

add_executable(test_production_ready
    test_production_ready.c
)

add_executable(test_vulnerability_scoring
    test_vulnerability_scoring.c
)

# Link libraries
target_link_libraries(test_ai_service
    ai-engine
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
)

target_link_libraries(test_integration
    ai-engine
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
)

target_link_libraries(test_production_ready
    ai-engine
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
)

target_link_libraries(test_vulnerability_scoring
    ai-engine
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
)

# Enable testing
enable_testing()

# Add tests
add_test(NAME ai_service_test COMMAND test_ai_service)
add_test(NAME integration_test COMMAND test_integration)
add_test(NAME production_ready_test COMMAND test_production_ready)
add_test(NAME vulnerability_scoring_test COMMAND test_vulnerability_scoring)

# Install test binaries (optional)
install(TARGETS test_ai_service test_integration test_production_ready test_vulnerability_scoring
    RUNTIME DESTINATION bin/tests
    OPTIONAL
)