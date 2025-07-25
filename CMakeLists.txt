# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.10)

project(ai-enhanced-openvas VERSION 1.0.0 LANGUAGES C)

message("-- Configuring AI-Enhanced OpenVAS...")

# Build options
option(BUILD_AI_ENGINE "Build AI integration engine" ON)
option(BUILD_AI_PLUGINS "Build AI-powered plugins" ON)
option(BUILD_CORE_COMPONENTS "Build core OpenVAS components" ON)
option(ENABLE_OPENAI "Enable OpenAI provider" ON)
option(ENABLE_CLAUDE "Enable Claude provider" OFF)
option(ENABLE_LOCAL_AI "Enable local AI models" OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)

# Global compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat -Wformat-security")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -DDEBUG")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DNDEBUG")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ai-engine)

# Build AI Engine
if(BUILD_AI_ENGINE)
    add_subdirectory(ai-engine)
endif()

# Build AI Plugins
if(BUILD_AI_PLUGINS)
    add_subdirectory(plugins)
endif()

# Build Tests
option(BUILD_TESTS "Build test suite" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Build Examples
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build Core Components (if requested)
if(BUILD_CORE_COMPONENTS)
    # Build order matters due to dependencies
    if(EXISTS "${CMAKE_SOURCE_DIR}/core/gvm-libs/CMakeLists.txt")
        add_subdirectory(core/gvm-libs)
    endif()
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/core/gvmd/CMakeLists.txt")
        add_subdirectory(core/gvmd)
    endif()
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/core/openvas-scanner/CMakeLists.txt")
        add_subdirectory(core/openvas-scanner)
    endif()
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/core/gsa/CMakeLists.txt")
        add_subdirectory(core/gsa)
    endif()
endif()

# Configuration summary
message(STATUS "")
message(STATUS "AI-Enhanced OpenVAS Configuration Summary:")
message(STATUS "==========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "AI Engine: ${BUILD_AI_ENGINE}")
message(STATUS "AI Plugins: ${BUILD_AI_PLUGINS}")
message(STATUS "Core Components: ${BUILD_CORE_COMPONENTS}")
message(STATUS "OpenAI Support: ${ENABLE_OPENAI}")
message(STATUS "Claude Support: ${ENABLE_CLAUDE}")
message(STATUS "Local AI Support: ${ENABLE_LOCAL_AI}")
message(STATUS "")

# Create configuration header
configure_file(
    "${CMAKE_SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h"
)

# Install configuration
install(FILES "${CMAKE_BINARY_DIR}/config.h"
    DESTINATION include/ai-enhanced-openvas
)