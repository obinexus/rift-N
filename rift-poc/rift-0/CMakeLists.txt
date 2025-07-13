# =================================================================
# RIFT Stage-0 CMake Build Configuration
# RIFT: RIFT Is a Flexible Translator
# OBINexus Computing Framework - AEGIS Compliant
# =================================================================

cmake_minimum_required(VERSION 3.16)

# Project definition with AEGIS compliance metadata
project(RIFT-Stage0 
    VERSION 0.1.0
    DESCRIPTION "RIFT Stage-0 Tokenization System - AEGIS Framework"
    LANGUAGES C
)

# =================================================================
# Build Options and Feature Flags
# =================================================================
option(ENABLE_QUANTUM_MODE "Enable quantum tokenization features" ON)
option(AEGIS_COMPLIANCE "Enable AEGIS framework compliance" ON)
option(ENABLE_PANIC_MODE "Enable panic failsafe mode" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_EXECUTABLES "Build executable targets" ON)
option(BUILD_TESTS "Build test suite" ON)
option(ENABLE_DUAL_MODE "Enable dual-mode [tb] parsing" ON)

# =================================================================
# Compiler Requirements and Standards
# =================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# POSIX compliance for features
add_definitions(-D_POSIX_C_SOURCE=200809L)
add_definitions(-D_GNU_SOURCE)

# =================================================================
# Directory Configuration
# =================================================================
set(RIFT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RIFT_INCLUDE_DIR ${RIFT_ROOT_DIR}/include)
set(RIFT_SOURCE_DIR ${RIFT_ROOT_DIR}/src)
set(RIFT_TEST_DIR ${RIFT_ROOT_DIR}/tests)
set(RIFT_BUILD_DIR ${CMAKE_BINARY_DIR})

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# =================================================================
# Include Custom CMake Modules
# =================================================================
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(rift_stage_macros)
include(RIFTStage0)

# =================================================================
# Compiler Flags Configuration
# =================================================================
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Base warning flags
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Wno-unused-function
    )
    
    # AEGIS compliance flags
    if(AEGIS_COMPLIANCE)
        add_compile_options(
            -Werror=implicit-function-declaration
            -Werror=incompatible-pointer-types
            -Werror=int-conversion
            -Wno-nested-externs
        )
    endif()
    
    # Debug/Release specific flags
    set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# =================================================================
# Find Required Dependencies
# =================================================================
find_package(Threads REQUIRED)

# Optional regex library
find_library(REGEX_LIB regex)
if(REGEX_LIB)
    message(STATUS "Found regex library: ${REGEX_LIB}")
else()
    message(STATUS "Using POSIX regex")
endif()

# =================================================================
# Generate Configuration Headers
# =================================================================
# Generate token types header
generate_token_types_header("${CMAKE_BINARY_DIR}/rift_token_types_gen.h")

# Generate token patterns header
generate_token_patterns("${CMAKE_BINARY_DIR}/rift_token_patterns_gen.h")

# Generate main configuration header
configure_file(
    "${RIFT_INCLUDE_DIR}/rift-0/core/config.h.in"
    "${CMAKE_BINARY_DIR}/rift_config.h"
    @ONLY
)

# Generate governance file
generate_governance_file(0)

# =================================================================
# Include Directories
# =================================================================
include_directories(
    ${RIFT_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# =================================================================
# Source File Collection
# =================================================================
# Core library sources
set(RIFT_CORE_SOURCES
    ${RIFT_SOURCE_DIR}/core/rift-0.c
    ${RIFT_SOURCE_DIR}/core/lexer/lexer.c
    ${RIFT_SOURCE_DIR}/core/lexer/lexer_flag.c
    ${RIFT_SOURCE_DIR}/core/lexer/tokenizer.c
    ${RIFT_SOURCE_DIR}/core/lexer/tokenizer_rules.c
    ${RIFT_SOURCE_DIR}/core/lexer/tokenizer_match.c
    ${RIFT_SOURCE_DIR}/core/lexer/tokenizer_utilities.c
    ${RIFT_SOURCE_DIR}/core/lexer/rift_tokenizer.c
    ${RIFT_SOURCE_DIR}/core/gov/rift-gov.0.c
    ${RIFT_SOURCE_DIR}/core/gov/r_governance_validation.c
    ${RIFT_SOURCE_DIR}/core/gov/stage_queue.c
    ${RIFT_SOURCE_DIR}/core/gov/rift_sim.c
    ${RIFT_SOURCE_DIR}/core/ext/r_uml.c
)

# Dual-mode parser sources (if enabled)
if(ENABLE_DUAL_MODE)
    list(APPEND RIFT_CORE_SOURCES
        ${RIFT_SOURCE_DIR}/core/parser/rift_tb_parser.c
    )
endif()

# CLI sources
set(RIFT_CLI_SOURCES
    ${RIFT_SOURCE_DIR}/cli/main.c
    ${RIFT_SOURCE_DIR}/cli/commands/lexer_command.c
    ${RIFT_SOURCE_DIR}/cli/commands/r_command.c
    ${RIFT_SOURCE_DIR}/cli/commands/ext_command.c
    ${RIFT_SOURCE_DIR}/cli/commands/rift_gov_command.c
)

# =================================================================
# Library Targets
# =================================================================
# Shared library
if(BUILD_SHARED_LIBS)
    add_library(rift-stage0 SHARED ${RIFT_CORE_SOURCES})
    set_target_properties(rift-stage0 PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
        OUTPUT_NAME "rift-0"
    )
    configure_stage_target(rift-stage0 0)
    target_link_libraries(rift-stage0 
        PUBLIC Threads::Threads
        PRIVATE ${CMAKE_DL_LIBS}
    )
    if(REGEX_LIB)
        target_link_libraries(rift-stage0 PRIVATE ${REGEX_LIB})
    endif()
endif()

# Static library
if(BUILD_STATIC_LIBS)
    add_library(rift-stage0-static STATIC ${RIFT_CORE_SOURCES})
    set_target_properties(rift-stage0-static PROPERTIES
        OUTPUT_NAME "rift-0"
    )
    configure_stage_target(rift-stage0-static 0)
    target_link_libraries(rift-stage0-static 
        PUBLIC Threads::Threads
    )
endif()

# Special .so.a archive (toolchain requirement)
add_library(rift-stage0-soa STATIC ${RIFT_CORE_SOURCES})
set_target_properties(rift-stage0-soa PROPERTIES
    OUTPUT_NAME "rift-0.so"
    SUFFIX ".a"
    POSITION_INDEPENDENT_CODE ON
)
configure_stage_target(rift-stage0-soa 0)

# =================================================================
# Executable Targets
# =================================================================
if(BUILD_EXECUTABLES)
    # Main RIFT-0 executable
    add_executable(rift-0 ${RIFT_CLI_SOURCES})
    if(BUILD_SHARED_LIBS)
        target_link_libraries(rift-0 PRIVATE rift-stage0)
    else()
        target_link_libraries(rift-0 PRIVATE rift-stage0-static)
    endif()
    configure_stage_target(rift-0 0)
    
    # Toolchain executables
    add_executable(riftlang.exe ${RIFT_SOURCE_DIR}/cli/main.c)
    set_target_properties(riftlang.exe PROPERTIES OUTPUT_NAME "riftlang")
    if(BUILD_SHARED_LIBS)
        target_link_libraries(riftlang.exe PRIVATE rift-stage0)
    else()
        target_link_libraries(riftlang.exe PRIVATE rift-stage0-static)
    endif()
    
    add_executable(rift.exe ${RIFT_SOURCE_DIR}/cli/main.c)
    set_target_properties(rift.exe PROPERTIES OUTPUT_NAME "rift")
    target_link_libraries(rift.exe PRIVATE rift-stage0-soa)
endif()

# =================================================================
# Test Configuration
# =================================================================
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# =================================================================
# Installation Rules
# =================================================================
# Headers
install(DIRECTORY ${RIFT_INCLUDE_DIR}/rift-0
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Generated headers
install(FILES
    ${CMAKE_BINARY_DIR}/rift_config.h
    ${CMAKE_BINARY_DIR}/rift_token_types_gen.h
    ${CMAKE_BINARY_DIR}/rift_token_patterns_gen.h
    DESTINATION include/rift-0/core
)

# Libraries
if(BUILD_SHARED_LIBS)
    install(TARGETS rift-stage0
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

if(BUILD_STATIC_LIBS)
    install(TARGETS rift-stage0-static rift-stage0-soa
        ARCHIVE DESTINATION lib
    )
endif()

# Executables
if(BUILD_EXECUTABLES)
    install(TARGETS rift-0 riftlang.exe rift.exe
        RUNTIME DESTINATION bin
    )
endif()

# Governance files
install(FILES ${CMAKE_BINARY_DIR}/gov.riftrc.0
    DESTINATION etc/rift
)

# pkg-config file
configure_file(
    ${RIFT_ROOT_DIR}/config/rift-0.pc.in
    ${CMAKE_BINARY_DIR}/rift-0.pc
    @ONLY
)
install(FILES ${CMAKE_BINARY_DIR}/rift-0.pc
    DESTINATION lib/pkgconfig
)

# =================================================================
# Custom Targets
# =================================================================
# Create toolchain pipeline
create_toolchain_pipeline()

# Build artifacts collection
create_build_artifacts()

# Clean all generated files
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/obj
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.h
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/gov.riftrc.*
    COMMENT "Cleaning all generated files"
)

# Validation target
add_custom_target(validate
    COMMAND ${CMAKE_COMMAND} -E echo "Validating RIFT Stage-0 build..."
    COMMAND ${CMAKE_COMMAND} -E echo "  CMake Version: ${CMAKE_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Compiler: ${CMAKE_C_COMPILER}"
    COMMAND ${CMAKE_COMMAND} -E echo "  AEGIS Compliance: ${AEGIS_COMPLIANCE}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Quantum Mode: ${ENABLE_QUANTUM_MODE}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Dual Mode [tb]: ${ENABLE_DUAL_MODE}"
    COMMENT "Build validation"
)

# Object file organization
add_custom_target(organize_objects ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/obj/core
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/obj/cli
    COMMENT "Organizing object file directories"
)

# =================================================================
# Status Messages
# =================================================================
message(STATUS "")
message(STATUS "RIFT Stage-0 Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  AEGIS Compliance: ${AEGIS_COMPLIANCE}")
message(STATUS "  Quantum Mode: ${ENABLE_QUANTUM_MODE}")
message(STATUS "  Dual Mode [tb]: ${ENABLE_DUAL_MODE}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# Print stage info
print_stage_info(0)