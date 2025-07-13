# RIFT Stage-0 CMake Build Configuration
# Part of AEGIS Project - OBINexus Computing
cmake_minimum_required(VERSION 3.16)
project(RIFT_Stage0 
    VERSION 0.1.0
    DESCRIPTION "RIFT Language Stage-0 Tokenizer with Dual-Channel Output"
    LANGUAGES C
)

# Policy: Enforce separation of core and CLI modules
# include/ → headers, src/core/ → core logic, src/cli/ → CLI logic

# ===================================================================
# Build Options
# ===================================================================
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_QUANTUM_MODE "Enable quantum channel processing" ON)
option(AEGIS_COMPLIANCE "Enable AEGIS governance compliance" ON)
option(BUILD_TESTS "Build test suite" ON)
option(ENABLE_PANIC_MODE "Enable panic mode for critical errors" ON)

# ===================================================================
# Compiler Configuration
# ===================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# ===================================================================
# Source Files
# ===================================================================

# Core sources
file(GLOB RIFT_CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.c"
)
# CLI sources
file(GLOB RIFT_CLI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cli/*.c"
)
# Headers
file(GLOB RIFT_STAGE0_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/rift-0/*.h"
)

# ===================================================================
# Build Directories
# ===================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)

# Create build directories
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# ===================================================================
# RIFT Stage-0 Core Library
# ===================================================================
add_library(rift-stage0 SHARED
    ${RIFT_CORE_SOURCES}
)

target_include_directories(rift-stage0 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(rift-stage0 PRIVATE
    Threads::Threads
    m  # Math library for potential quantum calculations
)

# Set library properties
set_target_properties(rift-stage0 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    OUTPUT_NAME "rift-stage0"
    POSITION_INDEPENDENT_CODE ON
)

# ===================================================================
# RIFT Stage-0 Executable
# ===================================================================
add_executable(riftlang
    ${RIFT_CLI_SOURCES}
)

target_link_libraries(riftlang PRIVATE
    rift-stage0
    Threads::Threads
)

# ===================================================================
# Additional Build Targets
# ===================================================================

# Static library (.a)
add_library(rift-stage0-static STATIC
    ${RIFT_CORE_SOURCES}
)

set_target_properties(rift-stage0-static PROPERTIES
    OUTPUT_NAME "rift-stage0"
    POSITION_INDEPENDENT_CODE ON
)

# Intermediate object for toolchain (.so.a)
add_custom_target(rift-stage0.so.a ALL
    COMMAND ${CMAKE_AR} rcs ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/rift-stage0.so.a 
            $<TARGET_FILE:rift-stage0-static>
    DEPENDS rift-stage0-static
    COMMENT "Creating intermediate .so.a archive"
)

# ===================================================================
# AEGIS Compliance Configuration
# ===================================================================
if(AEGIS_COMPLIANCE)
    target_compile_definitions(rift-stage0 PRIVATE
        AEGIS_COMPLIANT=1
        AEGIS_VERSION=0x0001
        ENABLE_GOVERNANCE=1
    )
    
    # Governance configuration file
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gov.riftrc.0.in
        ${CMAKE_BINARY_DIR}/gov.riftrc.0
        @ONLY
    )
endif()

# ===================================================================
# Quantum Mode Configuration
# ===================================================================
if(ENABLE_QUANTUM_MODE)
    target_compile_definitions(rift-stage0 PRIVATE
        QUANTUM_MODE_ENABLED=1
        DUAL_CHANNEL_OUTPUT=1
    )
endif()

# ===================================================================
# Error Level Configuration
# ===================================================================
target_compile_definitions(rift-stage0 PRIVATE
    WARNING_LEVEL_MIN=0
    WARNING_LEVEL_MAX=3
    DANGER_LEVEL_MIN=3
    DANGER_LEVEL_MAX=6
    CRITICAL_LEVEL_MIN=6
    CRITICAL_LEVEL_MAX=9
    PANIC_LEVEL_MIN=9
    PANIC_LEVEL_MAX=12
)

# ===================================================================
# Test Suite
# ===================================================================
if(BUILD_TESTS)
    enable_testing()
    
    # Basic tokenizer test
    add_executable(test_tokenizer
        tests/test_tokenizer.c
    )
    
    target_link_libraries(test_tokenizer PRIVATE
        rift-stage0
        Threads::Threads
    )
    
    add_test(NAME TokenizerBasic COMMAND test_tokenizer)
    
    # Dual channel test
    add_executable(test_dual_channel
        tests/test_dual_channel.c
    )
    
    target_link_libraries(test_dual_channel PRIVATE
        rift-stage0
        Threads::Threads
    )
    
    add_test(NAME DualChannelProcessing COMMAND test_dual_channel)
    
    # Memory governance test
    add_executable(test_memory_gov
        tests/test_memory_governance.c
    )
    
    target_link_libraries(test_memory_gov PRIVATE
        rift-stage0
        Threads::Threads
    )
    
    add_test(NAME MemoryGovernance COMMAND test_memory_gov)
    
    # QA test suite
    add_custom_target(qa_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        DEPENDS test_tokenizer test_dual_channel test_memory_gov
        COMMENT "Running QA test suite"
    )
endif()

# ===================================================================
# Installation
# ===================================================================
include(GNUInstallDirs)

install(TARGETS rift-stage0 rift-stage0-static riftlang
    EXPORT RiftStage0Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/rift-stage0.so.a
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    ${RIFT_STAGE0_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift-0
)

# ===================================================================
# Build Targets
# ===================================================================

# Main build target
add_custom_target(build_all ALL
    DEPENDS rift-stage0 rift-stage0-static riftlang rift-stage0.so.a
    COMMENT "Building all RIFT Stage-0 components"
)

######################################################################
# Modular policy: All new modules must be added to cmake/ and included here
# Example: include(cmake/SomePolicy.cmake)
######################################################################

# Clean build directory
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/obj
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/obj
    COMMENT "Cleaning build directories"
)

# ===================================================================
# Configuration Summary
# ===================================================================
message(STATUS "")
message(STATUS "RIFT Stage-0 Configuration Summary")
message(STATUS "==================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler:          ${CMAKE_C_COMPILER}")
message(STATUS "Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Options:")
message(STATUS "  Shared libs:     ${BUILD_SHARED_LIBS}")
message(STATUS "  Quantum mode:    ${ENABLE_QUANTUM_MODE}")
message(STATUS "  AEGIS compliant: ${AEGIS_COMPLIANCE}")
message(STATUS "  Build tests:     ${BUILD_TESTS}")
message(STATUS "  Panic mode:      ${ENABLE_PANIC_MODE}")
message(STATUS "")
message(STATUS "Toolchain:")
message(STATUS "  riftlang.exe → .so.a → rift.exe → gosilang")
message(STATUS "==================================")
message(STATUS "")