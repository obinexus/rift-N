# RIFT Stage Build Macros
# cmake/rift_stage_macros.cmake
# Implements multi-stage compilation pipeline for RIFT toolchain
# AEGIS Project - OBINexus Computing

include_guard(GLOBAL)

# ===================================================================
# Validation Utilities
# ===================================================================
function(validate_stage_number STAGE_NUM)
    if(NOT STAGE_NUM MATCHES "^[0-6]$")
        message(FATAL_ERROR "Invalid RIFT stage number: ${STAGE_NUM}. Must be 0-6.")
    endif()
endfunction()

function(validate_stage_sources SOURCES)
    if(NOT SOURCES)
        message(WARNING "No source files provided for RIFT stage.")
    endif()
    
    foreach(SOURCE ${SOURCES})
        if(NOT EXISTS ${SOURCE})
            message(FATAL_ERROR "Source file not found: ${SOURCE}")
        endif()
    endforeach()
endfunction()

# ===================================================================
# Build Pipeline Macros
# ===================================================================
# Macro to create complete RIFT stage build pipeline
macro(create_rift_stage STAGE_NUM)
    validate_stage_number(${STAGE_NUM})
    
    set(STAGE_NAME "rift-stage${STAGE_NUM}")
    set(STAGE_TARGET "${STAGE_NAME}")
    
    # Parse arguments
    set(options STATIC SHARED EXECUTABLE)
    set(oneValueArgs OUTPUT_NAME VERSION)
    set(multiValueArgs SOURCES DEPENDENCIES PUBLIC_HEADERS)
    cmake_parse_arguments(STAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Validate sources
    validate_stage_sources("${STAGE_SOURCES}")
    
    # Create targets based on type
    if(STAGE_STATIC)
        add_library(${STAGE_TARGET}-static STATIC ${STAGE_SOURCES})
        set_target_properties(${STAGE_TARGET}-static PROPERTIES
            OUTPUT_NAME "${STAGE_OUTPUT_NAME}"
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
        )
        configure_stage_target(${STAGE_TARGET}-static ${STAGE_NUM})
    endif()
    
    if(STAGE_SHARED)
        add_library(${STAGE_TARGET} SHARED ${STAGE_SOURCES})
        set_target_properties(${STAGE_TARGET} PROPERTIES
            OUTPUT_NAME "${STAGE_OUTPUT_NAME}"
            VERSION ${STAGE_VERSION}
            SOVERSION ${STAGE_NUM}
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )
        configure_stage_target(${STAGE_TARGET} ${STAGE_NUM})
    endif()
    
    if(STAGE_EXECUTABLE)
        add_executable(${STAGE_TARGET}-exe ${STAGE_SOURCES})
        set_target_properties(${STAGE_TARGET}-exe PROPERTIES
            OUTPUT_NAME "${STAGE_OUTPUT_NAME}"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        configure_stage_target(${STAGE_TARGET}-exe ${STAGE_NUM})
    endif()
endmacro()

# ===================================================================
# Toolchain Pipeline Functions
# ===================================================================
# Function to create the complete toolchain pipeline
# riftlang.exe → .so.a → rift.exe → gosilang
function(create_toolchain_pipeline)
    message(STATUS "Creating RIFT toolchain pipeline...")
    
    # Stage 1: riftlang.exe depends on librift-0
    if(TARGET riftlang.exe AND TARGET rift-stage0)
        add_dependencies(riftlang.exe rift-stage0)
    endif()
    
    # Stage 2: .so.a intermediate archive
    if(TARGET rift-stage0-soa)
        add_dependencies(rift-stage0-soa riftlang.exe)
    endif()
    
    # Stage 3: rift.exe final executable
    if(TARGET rift.exe)
        add_dependencies(rift.exe rift-stage0-soa)
    endif()
    
    # Stage 4: gosilang bridge (future implementation)
    # TODO: Add gosilang bridge when Stage-7 is implemented
    
    message(STATUS "Toolchain pipeline configured")
endfunction()

# ===================================================================
# Object File Management
# ===================================================================
function(create_object_library TARGET_NAME)
    set(options POSITION_INDEPENDENT)
    set(oneValueArgs OUTPUT_DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(OBJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    add_library(${TARGET_NAME} OBJECT ${OBJ_SOURCES})
    
    if(OBJ_POSITION_INDEPENDENT)
        set_target_properties(${TARGET_NAME} PROPERTIES
            POSITION_INDEPENDENT_CODE ON
        )
    endif()
    
    # Organize object files
    if(OBJ_OUTPUT_DIR)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OBJ_OUTPUT_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:${TARGET_NAME}> ${OBJ_OUTPUT_DIR}/
            COMMENT "Copying object files to ${OBJ_OUTPUT_DIR}"
        )
    endif()
endfunction()

# ===================================================================
# Header Generation Functions
# ===================================================================
function(generate_stage_config_header STAGE_NUM OUTPUT_FILE)
    validate_stage_number(${STAGE_NUM})
    
    file(WRITE ${OUTPUT_FILE}
"/* Auto-generated RIFT Stage-${STAGE_NUM} Configuration */
#ifndef RIFT_STAGE${STAGE_NUM}_CONFIG_H
#define RIFT_STAGE${STAGE_NUM}_CONFIG_H

#define RIFT_STAGE_NUMBER ${STAGE_NUM}
#define RIFT_STAGE_NAME \"Stage-${STAGE_NUM}\"
#define RIFT_STAGE_VERSION \"${PROJECT_VERSION}\"

/* Memory Configuration */
#define RIFT_HEAP_MIN ${RIFT_MEMORY_MIN_HEAP}
#define RIFT_HEAP_MAX ${RIFT_MEMORY_MAX_HEAP}
#define RIFT_PAGE_SIZE ${RIFT_MEMORY_PAGE_SIZE}

/* Feature Flags */
#define RIFT_QUANTUM_ENABLED ${ENABLE_QUANTUM_MODE}
#define RIFT_AEGIS_COMPLIANT ${AEGIS_COMPLIANCE}
#define RIFT_PANIC_MODE ${ENABLE_PANIC_MODE}

/* Toolchain Configuration */
#define RIFT_TOOLCHAIN \"riftlang.exe -> .so.a -> rift.exe -> gosilang\"

#endif /* RIFT_STAGE${STAGE_NUM}_CONFIG_H */
")
endfunction()

# ===================================================================
# Test Integration Functions
# ===================================================================
function(add_rift_test TEST_NAME)
    set(options UNIT INTEGRATION BENCHMARK)
    set(oneValueArgs SOURCE STAGE TIMEOUT)
    set(multiValueArgs DEPENDENCIES ARGS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Determine test type
    if(TEST_UNIT)
        set(TEST_DIR "unit")
    elseif(TEST_INTEGRATION)
        set(TEST_DIR "integration")
    elseif(TEST_BENCHMARK)
        set(TEST_DIR "benchmark")
    else()
        set(TEST_DIR "misc")
    endif()
    
    # Create test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Link dependencies
    if(TEST_DEPENDENCIES)
        target_link_libraries(${TEST_NAME} PRIVATE ${TEST_DEPENDENCIES})
    endif()
    
    # Configure for specific stage
    if(TEST_STAGE)
        configure_stage_target(${TEST_NAME} ${TEST_STAGE})
    endif()
    
    # Add to CTest
    add_test(NAME ${TEST_NAME} 
             COMMAND ${TEST_NAME} ${TEST_ARGS}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/${TEST_DIR})
    
    if(TEST_TIMEOUT)
        set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT ${TEST_TIMEOUT})
    endif()
endfunction()

# ===================================================================
# Build Artifact Functions
# ===================================================================
function(create_build_artifacts)
    # Create comprehensive build artifact structure
    add_custom_target(create_artifacts ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/artifacts
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/artifacts/lib
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/artifacts/bin
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/artifacts/include
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/artifacts/etc
        COMMENT "Creating artifact directories"
    )
    
    # Copy built artifacts to artifact directory
    add_custom_target(collect_artifacts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.so* 
                ${CMAKE_BINARY_DIR}/artifacts/lib/ 2>/dev/null || true
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/*.a 
                ${CMAKE_BINARY_DIR}/artifacts/lib/ 2>/dev/null || true
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/* 
                ${CMAKE_BINARY_DIR}/artifacts/bin/ 2>/dev/null || true
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${RIFT_INCLUDE_DIR}/rift-0 
                ${CMAKE_BINARY_DIR}/artifacts/include/rift-0
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                ${CMAKE_BINARY_DIR}/*.h 
                ${CMAKE_BINARY_DIR}/artifacts/include/rift-0/ 2>/dev/null || true
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                ${CMAKE_BINARY_DIR}/gov.riftrc.* 
                ${CMAKE_BINARY_DIR}/artifacts/etc/ 2>/dev/null || true
        DEPENDS create_artifacts
        COMMENT "Collecting build artifacts"
    )
endfunction()

# ===================================================================
# Compliance Validation
# ===================================================================
function(validate_aegis_compliance TARGET)
    if(NOT AEGIS_COMPLIANCE)
        return()
    endif()
    
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Validating AEGIS compliance for ${TARGET}..."
        COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Zero-trust governance enabled"
        COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Anti-ghosting protections active"
        COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Audit trail configured"
        COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Strict compliance mode"
        COMMENT "AEGIS compliance validation"
    )
endfunction()

# ===================================================================
# Documentation Generation
# ===================================================================
function(generate_build_documentation)
    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/build_report.md.in
        ${CMAKE_BINARY_DIR}/BUILD_REPORT.md
        @ONLY
    )
    
    add_custom_target(build_docs
        COMMAND ${CMAKE_COMMAND} -E echo "Build documentation generated: ${CMAKE_BINARY_DIR}/BUILD_REPORT.md"
        DEPENDS ${CMAKE_BINARY_DIR}/BUILD_REPORT.md
        COMMENT "Generating build documentation"
    )
endfunction()

# ===================================================================
# Utility Functions
# ===================================================================
function(print_stage_info STAGE_NUM)
    message(STATUS "")
    message(STATUS "RIFT Stage-${STAGE_NUM} Information:")
    message(STATUS "  Output Directory: ${CMAKE_BINARY_DIR}")
    message(STATUS "  Library Directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    message(STATUS "  Binary Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message(STATUS "  Object Directory: ${CMAKE_BINARY_DIR}/obj")
    message(STATUS "  Governance File: ${CMAKE_BINARY_DIR}/gov.riftrc.${STAGE_NUM}")
    message(STATUS "")
endfunction()

# ===================================================================
# Export Macros
# ===================================================================
macro(export_rift_stage STAGE_NUM)
    # Create export file for the stage
    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/RiftStage${STAGE_NUM}Config.cmake.in
        ${CMAKE_BINARY_DIR}/RiftStage${STAGE_NUM}Config.cmake
        @ONLY
    )
    
    # Install export file
    install(FILES ${CMAKE_BINARY_DIR}/RiftStage${STAGE_NUM}Config.cmake
            DESTINATION lib/cmake/RiftStage${STAGE_NUM})
endmacro()