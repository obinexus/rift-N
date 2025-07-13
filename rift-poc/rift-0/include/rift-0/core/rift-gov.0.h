

#ifndef RIFT_GOV_0_H
#define RIFT_GOV_0_H


#include <stddef.h>
#include <stdbool.h>
#if defined(_WIN32) || defined(_WIN64)
// POSIX pthreads are not available on Windows; provide minimal stubs for compatibility
typedef void* pthread_mutex_t;
#define pthread_mutex_init(mutex, attr)   (0)
#define pthread_mutex_destroy(mutex)      (0)
#define pthread_mutex_lock(mutex)         (0)
#define pthread_mutex_unlock(mutex)       (0)
#else
#include <pthread.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * Stage-0 Configuration Structure (from schema)
 * =================================================================== */
typedef struct {
    /* Memory governance */
    size_t min_heap_size;
    size_t max_heap_size;
    bool enable_dynamic_allocation;
    /* Channel configuration */
    bool enable_dual_channel;
    bool enable_quantum_mode;
    size_t classic_channel_size;
    size_t quantum_channel_size;
    /* Error handling */
    uint8_t default_error_level;
    bool enable_panic_mode;
    bool auto_fix_errors;
    /* AEGIS compliance */
    bool aegis_compliant;
    uint64_t compliance_flags;
    const char* governance_file;
    /* Threading */
    uint32_t thread_count;
    bool enable_parallel_tokenization;
} RiftStage0Config;

/* ===================================================================
 * Build Output Structure
 * =================================================================== */
typedef struct BuildOutput {
    char* obj_path;    // build/obj/*.o
    char* bin_path;    // build/bin/*
    char* lib_path;    // build/lib/*.so
    char* arc_path;    // build/lib/*.a
    bool build_success;
    uint32_t error_count;
    uint32_t warning_count;
} BuildOutput;

/* ===================================================================
 * Stage-0 Patterns (extern)
 * =================================================================== */
extern const TokenPattern stage0_patterns[];
extern const size_t stage0_patterns_count;

/* ===================================================================
 * Memory Governor API
 * =================================================================== */
TokenMemoryGovernor* create_memory_governor(size_t min_heap, size_t max_heap);
void* governed_malloc(TokenMemoryGovernor* gov, size_t size);
void governed_free(TokenMemoryGovernor* gov, void* ptr, size_t size);


/**
 * RIFT-0 Governance Compliance Header
 * Auto-generated for Stage-0 (Tokenization & Lexical Analysis)
 * Based on schema.json and AEGIS/PoliC governance requirements
 *
 * Ensures:
 *   - Governance metadata validation for rift-0
 *   - Tokenizer/lexer substage compliance
 *   - Stage 5 optimizer security hooks (future-proof)
 *   - Fallback and nlink integration
 *   - C API for rift-0 governance validation
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#if defined(_WIN32) || defined(_WIN64)
// POSIX pthreads are not available on Windows; provide minimal stubs for compatibility
typedef void* pthread_mutex_t;
#define pthread_mutex_init(mutex, attr)   (0)
#define pthread_mutex_destroy(mutex)      (0)
#define pthread_mutex_lock(mutex)         (0)
#define pthread_mutex_unlock(mutex)       (0)
#else
#include <pthread.h>
#endif


// AEGIS Governance Constants
#define MAX_PATH_LENGTH 512
#define MAX_STAGE_COUNT 7
#define MAX_SUBSTAGES_PER_STAGE 4
#define GOVERNANCE_EXPIRY_DAYS 90
#define SHA256_DIGEST_LENGTH 32

// Stage Type Enumeration
typedef enum {
    STAGE_TYPE_LEGACY = 0,
    STAGE_TYPE_EXPERIMENTAL = 1,
    STAGE_TYPE_STABLE = 2
} stage_type_t;

// Validation Result Codes
typedef enum {
    VALIDATION_SUCCESS = 0,
    VALIDATION_SCHEMA_VIOLATION = 1,
    VALIDATION_EXPIRED_GOVERNANCE = 2,
    VALIDATION_SEMVERX_VIOLATION = 3,
    VALIDATION_MISSING_GOVERNANCE = 4,
    VALIDATION_STAKEHOLDER_UNAUTHORIZED = 5,
    VALIDATION_CRITICAL_FAILURE = 6
} validation_result_t;

// Stakeholder Authorization Classes
typedef enum {
    STAKEHOLDER_USER = 1,
    STAKEHOLDER_DEVELOPER = 2,
    STAKEHOLDER_VENDOR = 4
} stakeholder_class_t;

// Governance Configuration Structure
typedef struct {
    char package_name[128];
    char version[32];
    char timestamp[32];
    int stage;
    stage_type_t stage_type;
    int semverx_lock;
    char entry_point[256];
    int nlink_enabled;
    stakeholder_class_t authorized_stakeholders;
} governance_config_t;

// Stage 5 Optimizer Security Structure
typedef struct {
    char optimizer_model[64];
    int minimization_verified;
    char path_hash[65];          // SHA-256 hex string + null terminator
    char post_optimization_hash[65];
    int audit_enabled;
    char security_level[32];
    int semantic_equivalence_proof;
} stage5_optimizer_t;

// Validation Context Structure
typedef struct {
    char project_root[MAX_PATH_LENGTH];
    int verbose_mode;
    int strict_mode;
    FILE *validation_log;
    governance_config_t stage_configs[MAX_STAGE_COUNT];
    int validated_stages;
} validation_context_t;

/* ===================================================================
 * Core Type Definitions (must be first)
 * =================================================================== */

typedef struct {
    const char* name;
    const char* pattern;
    int type; // Use RiftTokenType if available
    bool is_quantum;
} TokenPattern;

typedef struct TokenMemoryGovernor {
    size_t min_heap;
    size_t max_heap;
    size_t current_usage;
    bool dynamic_allowed;
    pthread_mutex_t mem_lock;
} TokenMemoryGovernor;

/* ===================================================================
 * RIFT Stage-0 Core Definitions
 * =================================================================== */

#define RIFT_VERSION_MAJOR 0
#define RIFT_VERSION_MINOR 1
#define RIFT_VERSION_PATCH 0
#define RIFT_STAGE_ID 0

/* Error severity levels */
typedef enum {
    RIFT_WARNING_MIN = 0,
    RIFT_WARNING_MAX = 3,
    RIFT_DANGER_MIN = 3,
    RIFT_DANGER_MAX = 6,
    RIFT_CRITICAL_MIN = 6,
    RIFT_CRITICAL_MAX = 9,
    RIFT_PANIC_MIN = 9,
    RIFT_PANIC_MAX = 12
} RiftErrorSeverity;

/* Token types - from R_INIT_EMPTY to complex patterns */
typedef enum {
    R_INIT_EMPTY = 0,
    R_IDENTIFIER,
    R_NUMBER,
    R_OPERATOR,
    R_KEYWORD,
    R_STRING,
    R_COMMENT,
    R_WHITESPACE,
    R_QUANTUM_TOKEN,
    R_COLLAPSE_MARKER,
    R_ENTANGLE_MARKER,
    R_GOVERNANCE_TAG,
    R_CUSTOM_PATTERN,
    R_EOF
} RiftTokenType;

/* Token memory governance */
typedef struct {
    size_t min_heap;
    size_t max_heap;
    size_t current_usage;
    bool dynamic_allowed;
    pthread_mutex_t mem_lock;
} TokenMemoryGovernor;

/* Dual-channel output structure */
typedef struct {
    void* classic_channel;
    size_t classic_size;
    void* quantum_channel;
    size_t quantum_size;
    uint8_t error_level;
    char* error_msg;
} DualChannelOutput;

/* Token structure with governance metadata */
typedef struct {
    RiftTokenType type;
    char* pattern;
    char* value;
    size_t line;
    size_t column;
    bool is_quantum;
    bool is_collapsed;
    uint64_t governance_flags;
    void* metadata;
} RiftToken;

/* Stage-0 tokenizer context */
#if defined(__unix__) || defined(__APPLE__)
#include <regex.h>
#else
// Provide a stub for regex_t if not available
typedef void* regex_t;
#endif

typedef struct {
    /* Core state */
    bool initialized;
    uint32_t stage_id;
    uint32_t version;
    
    /* Token patterns */
    regex_t* patterns;
    size_t pattern_count;
    
    /* Memory governance */
    TokenMemoryGovernor* mem_gov;
    
    /* Dual-channel configuration */
    bool dual_mode_enabled;
    bool quantum_mode_active;
    
    /* Error tracking */
    uint8_t current_error_level;
    char error_buffer[1024];
    
    /* AEGIS compliance */
    bool aegis_compliant;
    uint64_t compliance_flags;
    
    /* Thread management */
    pthread_mutex_t ctx_lock;
    uint32_t thread_count;
} RiftStage0Context;

/* ===================================================================
 * Token Pattern Definitions
 * =================================================================== */

typedef struct {
    const char* name;
    const char* pattern;
    RiftTokenType type;
    bool is_quantum;
} TokenPattern;

// --- Governance Config Structures (mirroring schema.json) ---

typedef enum {
    RIFT_STAGE_LEGACY = 0,
    RIFT_STAGE_EXPERIMENTAL = 1,
    RIFT_STAGE_STABLE = 2
} rift_stage_type_t;

typedef struct {
    char package_name[128];
    char version[32];
    char timestamp[32];
    int stage;
    rift_stage_type_t stage_type;
    char description[256];
    int semverx_lock;
    char entry_point[256];
    int nlink_enabled;
    // --- Tokenizer substage ---
    struct {
        int lexeme_validation;
        int token_memory_constraints;
        int encoding_normalization;
    } tokenizer;
    // --- Parser substage ---
    struct {
        int ast_depth_limit;
        char syntax_strictness[16];
        int error_recovery;
    } parser;
    // --- Semantic substage ---
    struct {
        int type_checking;
        int scope_validation;
        int symbol_table_integrity;
    } semantic;
    // --- Validator substage ---
    struct {
        int structural_acyclicity;
        int cost_bounds_enforced;
        int governance_hash_required;
    } validator;
    // --- Bytecode substage ---
    struct {
        int opcode_validation;
        int complexity_limits;
        int operand_alignment;
    } bytecode;
    // --- Verifier substage ---
    struct {
        int bytecode_integrity;
        int stack_safety;
        int memory_bounds;
    } verifier;
    // --- Emitter substage ---
    struct {
        char target_architecture[32];
        int optimization_level;
        int debug_symbols;
    } emitter;
} rift_gov_config_t;

// --- Stage 5 Optimizer Security (future-proof) ---
typedef struct {
    char optimizer_model[64];
    int minimization_verified;
    char path_hash[65];
    char post_optimization_hash[65];
    int audit_enabled;
    char security_level[32];
    char transformation_log[256];
    int semantic_equivalence_proof;
} rift_stage5_optimizer_t;

// --- Fallback Governance ---
typedef struct {
    int enabled;
    char fallback_directory[64];
    int experimental_bypass;
    int halt_on_critical;
} rift_fallback_gov_t;

// --- NLink Integration ---
typedef struct {
    int semverx_strict_mode;
    int hot_swap_validation;
    int component_lifecycle_tracking;
    int polybuild_coordination;
} rift_nlink_integration_t;

// --- API: Governance Validation for rift-0 ---

/**
 * Parse and validate rift-0 governance configuration from JSON file
 * @param file_path Path to governance JSON file
 * @param config Output: parsed config struct
 * @return 0 on success, nonzero on error
 */
int rift_gov0_parse_config(const char *file_path, rift_gov_config_t *config);

/**
 * Validate tokenization/lexical analysis governance for rift-0
 * @param config Parsed config struct
 * @return 0 if compliant, nonzero if violation
 */
int rift_gov0_validate_tokenizer(const rift_gov_config_t *config);

/**
 * Validate fallback governance policy
 * @param fallback Fallback config struct
 * @return 0 if fallback allowed, nonzero if not
 */
int rift_gov0_validate_fallback(const rift_fallback_gov_t *fallback);

/**
 * Validate nlink integration policy
 * @param nlink NLink config struct
 * @return 0 if compliant, nonzero if not
 */
int rift_gov0_validate_nlink(const rift_nlink_integration_t *nlink);

#ifdef __cplusplus
}
#endif

#endif 
