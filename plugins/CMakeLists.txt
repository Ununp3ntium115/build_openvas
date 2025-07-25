# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.10)

message("-- Configuring AI Plugins...")

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

# Vulnerability Analysis Plugin
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vulnerability-analysis/ai_vuln_analyzer.c")
    add_library(ai-vuln-analyzer SHARED
        vulnerability-analysis/ai_vuln_analyzer.c
    )
    
    target_link_libraries(ai-vuln-analyzer
        ai-engine
        ${GLIB_LIBRARIES}
        ${JSON_GLIB_LIBRARIES}
    )
    
    set_target_properties(ai-vuln-analyzer PROPERTIES
        VERSION 1.0.0
        SOVERSION 1
    )
    
    install(TARGETS ai-vuln-analyzer
        LIBRARY DESTINATION lib/ai-enhanced-openvas/plugins
    )
    
    message(STATUS "AI Vulnerability Analyzer plugin configured")
endif()

# Threat Modeling Plugin (placeholder)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threat-modeling/ai_threat_modeler.c")
    add_library(ai-threat-modeler SHARED
        threat-modeling/ai_threat_modeler.c
    )
    
    target_link_libraries(ai-threat-modeler
        ai-engine
        ${GLIB_LIBRARIES}
        ${JSON_GLIB_LIBRARIES}
    )
    
    install(TARGETS ai-threat-modeler
        LIBRARY DESTINATION lib/ai-enhanced-openvas/plugins
    )
    
    message(STATUS "AI Threat Modeler plugin configured")
endif()

# Scan Optimization Plugin (placeholder)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scan-optimization/ai_scan_optimizer.c")
    add_library(ai-scan-optimizer SHARED
        scan-optimization/ai_scan_optimizer.c
    )
    
    target_link_libraries(ai-scan-optimizer
        ai-engine
        ${GLIB_LIBRARIES}
        ${JSON_GLIB_LIBRARIES}
    )
    
    install(TARGETS ai-scan-optimizer
        LIBRARY DESTINATION lib/ai-enhanced-openvas/plugins
    )
    
    message(STATUS "AI Scan Optimizer plugin configured")
endif()

# Report Generation Plugin (placeholder)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/report-generation/ai_report_generator.c")
    add_library(ai-report-generator SHARED
        report-generation/ai_report_generator.c
    )
    
    target_link_libraries(ai-report-generator
        ai-engine
        ${GLIB_LIBRARIES}
        ${JSON_GLIB_LIBRARIES}
    )
    
    install(TARGETS ai-report-generator
        LIBRARY DESTINATION lib/ai-enhanced-openvas/plugins
    )
    
    message(STATUS "AI Report Generator plugin configured")
endif()

# Install plugin headers
install(FILES
    vulnerability-analysis/ai_vuln_analyzer.h
    DESTINATION include/ai-enhanced-openvas/plugins
    OPTIONAL
)