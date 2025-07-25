# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.10)

message("-- Configuring AI-Enhanced OpenVAS Examples...")

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

# AI Vulnerability Demo
add_executable(ai_vulnerability_demo
    ai_vulnerability_demo.c
)

target_link_libraries(ai_vulnerability_demo
    ai-engine
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
)

# Install examples (optional)
install(TARGETS ai_vulnerability_demo
    RUNTIME DESTINATION bin/examples
    OPTIONAL
)

# Install example data files
install(FILES
    README_EXAMPLES.md
    DESTINATION share/doc/ai-enhanced-openvas/examples
    OPTIONAL
)