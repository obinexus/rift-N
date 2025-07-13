# RIFT Stage-0 Specific CMake Configuration
# cmake/RIFTStage0.cmake
# Part of AEGIS Project - OBINexus Computing

include_guard(GLOBAL)

# ===================================================================
# Stage-0 Token Type Definitions
# ===================================================================
set(RIFT_TOKEN_TYPES
    R_INIT_EMPTY
    R_IDENTIFIER
    R_NUMBER
    R_OPERATOR
    R_KEYWORD
    R_STRING
    R_COMMENT
    R_WHITESPACE
    R_QUANTUM_TOKEN
    R_COLLAPSE_MARKER
    R_ENTANGLE_MARKER
    R_GOVERNANCE_TAG
    R_CUSTOM_PATTERN
    R_EOF
)

# Generate token type enum header
function(generate_token_types_header OUTPUT_FILE)
    set(HEADER_CONTENT "#ifndef RIFT_TOKEN_TYPES_GEN_H
#define RIFT_TOKEN_TYPES_GEN_H

typedef enum {
")
    set(INDEX 0)
    foreach(TOKEN_TYPE ${RIFT_TOKEN_TYPES})
        string(APPEND HEADER_CONTENT "    ${TOKEN_TYPE} = ${INDEX},\n")
        math(EXPR INDEX "${INDEX} + 1")
    endforeach()
    
    string(APPEND HEADER_CONTENT "    R_TOKEN_TYPE_COUNT
} RiftTokenType;

#endif /* RIFT_TOKEN_TYPES_GEN_H */
")
    
    file(WRITE ${OUTPUT_FILE} ${HEADER_CONTENT})
endfunction()

# ===================================================================
# Token Pattern Configuration
# ===================================================================
function(generate_token_patterns OUTPUT_FILE)
    file(WRITE ${OUTPUT_FILE}
"/* Auto-generated token pattern definitions */
#ifndef RIFT_TOKEN_PATTERNS_H
#define RIFT_TOKEN_PATTERNS_H

#include \"rift-0/core/lexer/tokenizer_types.h\"

typedef struct {
    const char* name;
    const char* pattern;
    RiftTokenType type;
    int is_quantum;
    int priority;
} TokenPatternDef;

static const TokenPatternDef stage0_patterns[] = {
    /* Classical patterns */
    {\"identifier\", \"^[a-zA-Z_][a-zA-Z0-9_]*$\", R_IDENTIFIER, 0, 100},
    {\"number\", \"^[0-9]+(\\\\.[0-9]+)?$\", R_NUMBER, 0, 90},
    {\"operator\", \"^[+\\\\-*/=<>!&|^~%]$\", R_OPERATOR, 0, 80},
    {\"string\", \"^\\\"[^\\\"]*\\\"$\", R_STRING, 0, 85},
    {\"comment\", \"^//.*$\", R_COMMENT, 0, 70},
    {\"whitespace\", \"^[ \\\\t\\\\n\\\\r]+$\", R_WHITESPACE, 0, 60},
    
    /* Quantum patterns */
    {\"quantum_init\", \"^@quantum\\\\s*\\\\{$\", R_QUANTUM_TOKEN, 1, 150},
    {\"collapse\", \"^!collapse$\", R_COLLAPSE_MARKER, 1, 140},
    {\"entangle\", \"^@entangle$\", R_ENTANGLE_MARKER, 1, 130},
    
    /* Governance patterns */
    {\"governance\", \"^#\\\\[gov:[^\\\\]]+\\\\]$\", R_GOVERNANCE_TAG, 0, 120},
    
    /* Terminator */
    {NULL, NULL, R_INIT_EMPTY, 0, 0}
};

extern const size_t stage0_patterns_count;

#endif /* RIFT_TOKEN_PATTERNS_H */
")
endfunction()

# ===================================================================
# Memory Governance Configuration
# ===================================================================
set(RIFT_MEMORY_MIN_HEAP "1048576" CACHE STRING "Minimum heap size (1MB)")
set(RIFT_MEMORY_MAX_HEAP "16777216" CACHE STRING "Maximum heap size (16MB)")
set(RIFT_MEMORY_SCHEDULER "minmax" CACHE STRING "Memory scheduler type")
set(RIFT_MEMORY_PAGE_SIZE "4096" CACHE STRING "Memory page size")

# ===================================================================
# Error Level Configuration
# ===================================================================
function(configure_error_levels TARGET)
    target_compile_definitions(${TARGET} PRIVATE
        # Warning levels (0-3): Yellow platter
        RIFT_WARNING_MIN=0
        RIFT_WARNING_MAX=3
        RIFT_WARNING_COLOR="YELLOW"
        
        # Danger levels (3-6): Orange region  
        RIFT_DANGER_MIN=3
        RIFT_DANGER_MAX=6
        RIFT_DANGER_COLOR="ORANGE"
        
        # Critical levels (6-9): Red alert
        RIFT_CRITICAL_MIN=6
        RIFT_CRITICAL_MAX=9
        RIFT_CRITICAL_COLOR="RED"
        
        # Panic levels (9-12): Failsafe mode
        RIFT_PANIC_MIN=9
        RIFT_PANIC_MAX=12
        RIFT_PANIC_COLOR="PURPLE"
        RIFT_PANIC_FAILSAFE=1
    )
endfunction()

# ===================================================================
# Dual Channel Configuration
# ===================================================================
function(configure_dual_channel TARGET)
    target_compile_definitions(${TARGET} PRIVATE
        DUAL_CHANNEL_ENABLED=1
        CHANNEL_CLASSIC=1
        CHANNEL_QUANTUM=$<BOOL:${ENABLE_QUANTUM_MODE}>
        CHANNEL_ISOLATION_STRICT=1
        CHANNEL_BUFFER_SIZE=8192
    )
    
    if(ENABLE_QUANTUM_MODE)
        target_compile_definitions(${TARGET} PRIVATE
            QUANTUM_COHERENCE_CHECK=1
            QUANTUM_COLLAPSE_DETECTION=1
            QUANTUM_ENTANGLEMENT_TRACKING=1
            QUANTUM_SUPERPOSITION_LIMIT=8
        )
    endif()
endfunction()

# ===================================================================
# AEGIS Compliance Configuration  
# ===================================================================
function(configure_aegis_compliance TARGET)
    if(AEGIS_COMPLIANCE)
        target_compile_definitions(${TARGET} PRIVATE
            AEGIS_COMPLIANT=1
            AEGIS_VERSION=0x0001
            AEGIS_ZERO_TRUST=1
            AEGIS_ANTI_GHOSTING=1
            AEGIS_AUDIT_TRAIL=1
            AEGIS_STRICT_GOVERNANCE=1
        )
        
        # Add AEGIS-specific compiler flags
        target_compile_options(${TARGET} PRIVATE
            -Werror=implicit-function-declaration
            -Werror=incompatible-pointer-types
            -Werror=int-conversion
            -Wno-nested-externs
        )
    endif()
endfunction()

# ===================================================================
# Governance File Generation
# ===================================================================
function(generate_governance_file STAGE_ID)
    set(GOV_FILE "${CMAKE_BINARY_DIR}/gov.riftrc.${STAGE_ID}")
    
    file(WRITE ${GOV_FILE}
"# RIFT Stage-${STAGE_ID} Governance Configuration
# Auto-generated by CMake build system
# AEGIS Project Compliance Level: STRICT

[stage]
id = ${STAGE_ID}
version = ${PROJECT_VERSION}
compliance = aegis
toolchain = \"riftlang.exe -> .so.a -> rift.exe -> gosilang\"

[memory]
min_heap = ${RIFT_MEMORY_MIN_HEAP}
max_heap = ${RIFT_MEMORY_MAX_HEAP}
page_size = ${RIFT_MEMORY_PAGE_SIZE}
scheduler = \"${RIFT_MEMORY_SCHEDULER}\"
gc_enabled = false
zero_init = true

[error_handling]
warning_range = \"0-3\"
danger_range = \"3-6\"
critical_range = \"6-9\"
panic_range = \"9-12\"
auto_panic_failsafe = ${ENABLE_PANIC_MODE}
stack_trace = true

[channels]
classic_enabled = true
quantum_enabled = ${ENABLE_QUANTUM_MODE}
isolation_level = \"strict\"
buffer_size = 8192

[governance]
zero_trust = true
anti_ghosting = true
audit_trail = \"enabled\"
compliance_level = \"strict\"
signature_required = true

[build]
timestamp = \"${CMAKE_CURRENT_BINARY_DIR}\"
compiler = \"${CMAKE_C_COMPILER}\"
flags = \"${CMAKE_C_FLAGS}\"
")
    
    message(STATUS "Generated governance file: ${GOV_FILE}")
endfunction()

# ===================================================================
# Stage Configuration Macro
# ===================================================================
macro(configure_stage_target TARGET STAGE_NUM)
    validate_stage_number(${STAGE_NUM})
    
    # Apply all configurations
    configure_error_levels(${TARGET})
    configure_dual_channel(${TARGET})
    configure_aegis_compliance(${TARGET})
    
    # Stage-specific definitions
    target_compile_definitions(${TARGET} PRIVATE
        RIFT_STAGE=${STAGE_NUM}
        RIFT_STAGE_NAME="Stage-${STAGE_NUM}"
        RIFT_VERSION="${PROJECT_VERSION}"
    )
    
    # Link required libraries
    target_link_libraries(${TARGET} PRIVATE
        Threads::Threads
        ${CMAKE_DL_LIBS}
    )
endmacro()

# ===================================================================
# Object File Organization
# ===================================================================
function(organize_object_files TARGET)
    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    if(TARGET_TYPE STREQUAL "OBJECT_LIBRARY" OR 
       TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory 
                    ${CMAKE_BINARY_DIR}/obj/$<TARGET_NAME:${TARGET}>
            COMMAND ${CMAKE_COMMAND} -E copy 
                    $<TARGET_OBJECTS:${TARGET}>
                    ${CMAKE_BINARY_DIR}/obj/$<TARGET_NAME:${TARGET}>/
            COMMENT "Organizing object files for ${TARGET}"
        )
    endif()
endfunction()

# ===================================================================
# Generate Required Headers
# ===================================================================
generate_token_types_header("${CMAKE_BINARY_DIR}/rift_token_types_gen.h")
generate_token_patterns("${CMAKE_BINARY_DIR}/rift_token_patterns_gen.h")

# ===================================================================
# Export Configuration
# ===================================================================
set(RIFT_STAGE0_FOUND TRUE)
set(RIFT_STAGE0_VERSION "${PROJECT_VERSION}")
set(RIFT_STAGE0_INCLUDE_DIRS "${RIFT_INCLUDE_DIR};${CMAKE_BINARY_DIR}")
set(RIFT_STAGE0_LIBRARIES "rift-stage0")
set(RIFT_STAGE0_DEFINITIONS 
    RIFT_STAGE0_AVAILABLE=1
    RIFT_VERSION="${PROJECT_VERSION}"
)