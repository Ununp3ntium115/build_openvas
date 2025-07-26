# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.10)

project(ai-engine VERSION 1.0.0 LANGUAGES C)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)
pkg_check_modules(CURL REQUIRED libcurl)

# Include directories
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${JSON_GLIB_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLIB_CFLAGS_OTHER}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JSON_GLIB_CFLAGS_OTHER}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CURL_CFLAGS_OTHER}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11")

# AI Engine Core Library
set(AI_ENGINE_SOURCES
    api/ai_service.c
    providers/openai_provider.c
    config/ai_config_manager.c
    security/ai_security.c
    monitoring/ai_metrics.c
    scoring/vulnerability_scoring.c
    scoring/external_data_sources.c
)

set(AI_ENGINE_HEADERS
    api/ai_service.h
    config/ai_config_manager.h
    security/ai_security.h
    monitoring/ai_metrics.h
    scoring/vulnerability_scoring.h
)

# Create AI Engine library
add_library(ai-engine SHARED ${AI_ENGINE_SOURCES})
target_link_libraries(ai-engine 
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
    ${CURL_LIBRARIES}
)

# Set library properties
set_target_properties(ai-engine PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${AI_ENGINE_HEADERS}"
)

# Installation
install(TARGETS ai-engine
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/ai-engine
)

# Install additional headers
install(FILES api/ai_service.h
    DESTINATION include/ai-engine
)

# Create pkg-config file
configure_file(ai-engine.pc.in ai-engine.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/ai-engine.pc
    DESTINATION lib/pkgconfig
)