# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.10)

project(openvas-report-generation
  VERSION 1.0.0
  DESCRIPTION "Professional LaTeX Report Generation Plugin for OpenVAS"
  LANGUAGES C)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.50)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0>=1.4)

# Check for LaTeX installation
find_program(PDFLATEX_EXECUTABLE pdflatex)
if(NOT PDFLATEX_EXECUTABLE)
    message(WARNING "pdflatex not found. LaTeX reports will not be available.")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ai-engine/scoring
    ${GLIB_INCLUDE_DIRS}
    ${JSON_GLIB_INCLUDE_DIRS}
)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLIB_CFLAGS} ${JSON_GLIB_CFLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

# Source files
set(REPORT_GENERATION_SOURCES
    src/latex_engine.c
    src/template_manager.c
    src/report_api.c
    src/chart_generator.c
    src/data_processor.c
    src/output_manager.c
)

set(REPORT_GENERATION_HEADERS
    src/latex_engine.h
    src/template_manager.h
    src/report_api.h
    src/chart_generator.h
    src/data_processor.h
    src/output_manager.h
)

# Create the report generation library
add_library(openvas-report-generation SHARED
    ${REPORT_GENERATION_SOURCES}
)

# Link libraries
target_link_libraries(openvas-report-generation
    ${GLIB_LIBRARIES}
    ${JSON_GLIB_LIBRARIES}
)

# Set library properties
set_target_properties(openvas-report-generation PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${REPORT_GENERATION_HEADERS}"
)

# Installation
install(TARGETS openvas-report-generation
    LIBRARY DESTINATION lib/openvas/plugins
    PUBLIC_HEADER DESTINATION include/openvas/plugins/report-generation
)

# Install templates
install(DIRECTORY templates/
    DESTINATION share/openvas/report-templates
    FILES_MATCHING 
    PATTERN "*.tex"
    PATTERN "*.json"
    PATTERN "*.sty"
)

# Install assets
install(DIRECTORY assets/
    DESTINATION share/openvas/report-assets
    FILES_MATCHING
    PATTERN "*.png"
    PATTERN "*.jpg"
    PATTERN "*.pdf"
    PATTERN "*.sty"
)

# Create configuration directory
install(DIRECTORY DESTINATION etc/openvas/report-generation)

# Install default configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/report-generation.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/report-generation.conf
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/report-generation.conf
    DESTINATION etc/openvas/report-generation
)

# Testing
enable_testing()

# Unit tests
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Documentation
find_program(DOXYGEN_EXECUTABLE doxygen)
if(DOXYGEN_EXECUTABLE)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )
    
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Package configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/openvas-report-generation.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/openvas-report-generation.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/openvas-report-generation.pc
    DESTINATION lib/pkgconfig
)

# Print configuration summary
message(STATUS "")
message(STATUS "OpenVAS Report Generation Plugin Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  LaTeX support: ${PDFLATEX_EXECUTABLE}")
message(STATUS "  GLib version: ${GLIB_VERSION}")
message(STATUS "  JSON-GLib version: ${JSON_GLIB_VERSION}")
message(STATUS "")