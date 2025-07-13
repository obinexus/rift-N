#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* Project headers */
#include "rift-0/core/lexer/tokenizer_types.h"




/* RIFT_CLEANUP: Forward declarations to resolve type issues */
#ifndef RIFT_CLEANUP_FORWARD_DECLS
#define RIFT_CLEANUP_FORWARD_DECLS

/* Forward declare missing types if not already defined */
#ifndef RIFT_TOKEN_TYPE_DEFINED
#define RIFT_TOKEN_TYPE_DEFINED
#endif

/* Resolve HeapQueue issues in lexer_flag.h */
#ifndef HEAPQUEUE_FORWARD_DECLARED
#define HEAPQUEUE_FORWARD_DECLARED
struct HeapQueue;
typedef struct HeapQueue HeapQueue;

typedef struct {
    int priority;
    int value;
} Item;
#endif

/* Add strdup declaration if missing */
#ifndef HAVE_STRDUP
#ifdef __cplusplus
extern "C" {
#endif
char* strdup(const char* s);
#ifdef __cplusplus
}
#endif
#endif

#endif /* RIFT_CLEANUP_FORWARD_DECLS */



/* RIFT_CLEANUP: Forward declarations to resolve type issues */
#ifndef RIFT_CLEANUP_FORWARD_DECLS
#define RIFT_CLEANUP_FORWARD_DECLS

/* Forward declare missing types if not already defined */
#ifndef RIFT_TOKEN_TYPE_DEFINED
#define RIFT_TOKEN_TYPE_DEFINED
#endif

/* Resolve HeapQueue issues in lexer_flag.h */
#ifndef HEAPQUEUE_FORWARD_DECLARED
#define HEAPQUEUE_FORWARD_DECLARED
struct HeapQueue;
typedef struct HeapQueue HeapQueue;

typedef struct {
    int priority;
    int value;
} Item;
#endif

/* Add strdup declaration if missing */
#ifndef HAVE_STRDUP
#ifdef __cplusplus
extern "C" {
#endif
char* strdup(const char* s);
#ifdef __cplusplus
}
#endif
#endif

#endif /* RIFT_CLEANUP_FORWARD_DECLS */




// --- Thread-safe HeapQueue implementation ---
HeapQueue* hq_create(int capacity) {
    HeapQueue* hq = (HeapQueue*)malloc(sizeof(HeapQueue));
    hq->capacity = capacity;
    hq->size = 0;
    hq->heap = (Item*)malloc(capacity * sizeof(Item));
    pthread_mutex_init(&hq->lock, NULL);
    return hq;
}

void hq_destroy(HeapQueue* hq) {
    if (!hq) return;
    free(hq->heap);
    pthread_mutex_destroy(&hq->lock);
    free(hq);
}

void hq_enqueue(HeapQueue* hq, int priority, int value) {
    pthread_mutex_lock(&hq->lock);
    if (hq->size == hq->capacity) {
        fprintf(stderr, "HeapQueue is full. Cannot enqueue.\n");
        pthread_mutex_unlock(&hq->lock);
        return;
    }
    Item item = { priority, value };
    int i = hq->size++;
    int parent = (i - 1) / 2;
    while (i > 0 && hq->heap[parent].priority > item.priority) {
        hq->heap[i] = hq->heap[parent];
        i = parent;
        parent = (i - 1) / 2;
    }
    hq->heap[i] = item;
    pthread_mutex_unlock(&hq->lock);
}

Item hq_dequeue(HeapQueue* hq) {
    pthread_mutex_lock(&hq->lock);
    if (hq->size == 0) {
        fprintf(stderr, "HeapQueue is empty. Cannot dequeue.\n");
        pthread_mutex_unlock(&hq->lock);
        return (Item){0, 0};
    }
    Item root = hq->heap[0];
    Item last = hq->heap[--hq->size];
    int i = 0;
    int child = 1;
    while (child < hq->size) {
        if (child + 1 < hq->size && hq->heap[child + 1].priority < hq->heap[child].priority) {
            child++;
        }
        if (last.priority <= hq->heap[child].priority) {
            break;
        }
        hq->heap[i] = hq->heap[child];
        i = child;
        child = 2 * i + 1;
    }
    hq->heap[i] = last;
    pthread_mutex_unlock(&hq->lock);
    return root;
}

// --- Lexer flag management ---
void lexer_set_flag(LexerContext* ctx, LexerFlags flag) {
    ctx->flags |= flag;
}

void lexer_clear_flag(LexerContext* ctx, LexerFlags flag) {
    ctx->flags &= ~flag;
}

bool lexer_flag_enabled(const LexerContext* ctx, LexerFlags flag) {
    return (ctx->flags & flag) != 0;
}


// --- Modular token creation ---
TokenNode* token_create(TokenType type, const char* value, size_t length) {
    TokenNode* token = (TokenNode*)malloc(sizeof(TokenNode));
    token->type = type;
    token->length = length;
    token->value = (char*)malloc(length + 1);
    memcpy(token->value, value, length);
    token->value[length] = '\0';
    token->memory = NULL;
    return token;
}

void token_destroy(TokenNode* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}


// --- Example: heap-based top-down and shift-reduce matching ---
void demo_lexer_flags() {
    LexerContext ctx = {0};
    lexer_set_flag(&ctx, LEXER_FLAG_RAW_STRING);
    lexer_set_flag(&ctx, LEXER_FLAG_GLOBAL_TB);
    lexer_set_flag(&ctx, LEXER_FLAG_TOP_DOWN);

    if (lexer_flag_enabled(&ctx, LEXER_FLAG_RAW_STRING)) {
        printf("Raw string mode enabled\n");
    }
    if (lexer_flag_enabled(&ctx, LEXER_FLAG_GLOBAL_TB)) {
        printf("Global [tb] matching enabled\n");
    }
    if (lexer_flag_enabled(&ctx, LEXER_FLAG_TOP_DOWN)) {
        printf("Top-down parsing enabled\n");
    }
    lexer_clear_flag(&ctx, LEXER_FLAG_TOP_DOWN);
    lexer_set_flag(&ctx, LEXER_FLAG_SHIFT_REDUCE);
    if (lexer_flag_enabled(&ctx, LEXER_FLAG_SHIFT_REDUCE)) {
        printf("Shift-reduce parsing enabled\n");
    }
}

// --- Example: type-safe token creation ---
void demo_token_creation() {
    TokenNode* t1 = token_create(TOKEN_TYPE_RAW_STRING, "R\"example\"", 10);
    printf("Token type: %d, value: %s\n", t1->type, t1->value);
    token_destroy(t1);
}
 * contract enforcement for RIFTLang parallelism and concurrency system.
 * 
 * Supports:
 * - Full scope envelope validation for Gate 1 PolicyValidationRatio = 85%
 * - Memory segment boundary enforcement (span<row>, span<fixed>, span<superposed>)
 * - R/W/X policy validation bitfields
 * - Parent thread authority inheritance markers
 * - Integration with existing rift_telemetry.c infrastructure
 */

#ifndef TOKEN_ACCESS_ENVELOPE_H
#define TOKEN_ACCESS_ENVELOPE_H

/* RIFT_CLEANUP: Commented out missing header
 * #include "rift_common.h"
 */

// =============================================================================
// MEMORY SEGMENT BOUNDARY DEFINITIONS
// =============================================================================

/**
 * @brief Memory span type classifications for governance contract validation
 */
typedef enum {
    SPAN_ROW,          // Ordered, expandable contexts with explicit anchors
    SPAN_FIXED,        // Singleton authority with permanent role binding
    SPAN_SUPERPOSED    // Tokens existing in multiple states simultaneously
} rift_span_type_t;

/**
 * @brief Memory segment boundary structure for governance enforcement
 */
typedef struct {
    rift_span_type_t span_type;     // Memory span classification
    void* memory_base;              // Base memory address
    size_t segment_size;            // Segment size in bytes
    uint32_t alignment_bits;        // Memory alignment requirements (4096 for classical, 8 for quantum)
    bool is_mutable;                // Mutability flag for governance enforcement
    uint64_t parent_segment_id;     // Parent segment for inheritance validation
} rift_memory_segment_t;

// =============================================================================
// POLICY VALIDATION BITFIELD DEFINITIONS
// =============================================================================

/**
 * @brief Access permission flags for R/W/X policy validation
 */
#define RIFT_ACCESS_READ    0x01    // Read permission
#define RIFT_ACCESS_WRITE   0x02    // Write permission  
#define RIFT_ACCESS_EXECUTE 0x04    // Execute permission
#define RIFT_ACCESS_CREATE  0x08    // Create new resources permission
#define RIFT_ACCESS_DELETE  0x10    // Delete resources permission
#define RIFT_ACCESS_INHERIT 0x20    // Inherit permissions to child threads

/**
 * @brief Policy validation matrix for governance contract enforcement
 */
typedef struct {
    uint32_t access_permissions;    // Bitfield of allowed access operations
    uint32_t restricted_operations; // Bitfield of explicitly denied operations
    uint64_t policy_version;        // Policy version for compatibility validation
    char policy_name[64];           // Human-readable policy identifier
    struct timespec policy_expiry;  // Policy expiration timestamp
} rift_policy_matrix_t;

// =============================================================================
// PARENT THREAD AUTHORITY INHERITANCE
// =============================================================================

/**
 * @brief Thread authority inheritance markers for governance chain validation
 */
typedef struct {
    uint64_t parent_thread_id;      // Parent RIFT thread identifier
    uint64_t authority_chain_depth; // Depth in authority inheritance chain
    rift_policy_matrix_t inherited_policy; // Policy inherited from parent
    uint32_t authority_restrictions; // Additional restrictions applied at this level
    bool can_delegate_authority;    // Whether this thread can create child threads
    uint32_t max_child_threads;     // Maximum number of child threads allowed
} rift_thread_authority_t;

// =============================================================================
// TOKEN ACCESS ENVELOPE STRUCTURE
// =============================================================================

/**
 * @brief Comprehensive token access envelope for governance contract validation
 * 
 * This structure implements the full scope envelope as specified for Phase 1
 * implementation, supporting Gate 1 PolicyValidationRatio requirements.
 */
typedef struct {
    // Memory segment boundaries
    rift_memory_segment_t* accessible_segments;
    uint32_t segment_count;
    
    // Policy validation matrix
    rift_policy_matrix_t policy;
    
    // Thread authority inheritance
    rift_thread_authority_t authority;
    
    // Envelope metadata
    uint64_t envelope_id;           // Unique envelope identifier
    struct timespec creation_time;  // Envelope creation timestamp
    uint64_t creator_thread_id;     // Thread that created this envelope
    uint32_t validation_checksum;   // Integrity validation checksum
    bool is_validated;              // Validation status flag
    
    // Governance contract enforcement
    pthread_mutex_t envelope_mutex; // Thread-safe access control
    uint32_t access_violation_count; // Number of access violations detected
    char violation_log[256];        // Brief violation history
} rift_token_access_envelope_t;

// =============================================================================
// JOB CONTEXT EXTENSION STRUCTURE
// =============================================================================

/**
 * @brief Extended job context structure integrating with existing telemetry
 * 
 * Extends rift_spawn_telemetry_t as specified in technical direction to
 * leverage proven PID/TID tracking and hierarchical spawn validation.
 */
typedef struct {
    rift_spawn_telemetry_t base_telemetry;  // Inherit existing tracking
    rift_governance_policy_t job_policy;    // Job-specific governance policy
    uint64_t job_hydration_id;              // Unique job identifier
    rift_token_access_envelope_t envelope;  // Token access boundaries
    
    // Job lifecycle management
    enum {
        JOB_STATE_CREATED,
        JOB_STATE_HYDRATED,
        JOB_STATE_DISPATCHED,
        JOB_STATE_EXECUTING,
        JOB_STATE_COMPLETED,
        JOB_STATE_FAILED,
        JOB_STATE_TERMINATED
    } job_state;
    
    // Cooperative multitasking support
    struct timespec last_yield_time;        // Last cooperative yield timestamp
    uint32_t governance_checkpoint_count;   // Number of governance checkpoints passed
    bool yield_requested;                   // Cooperative yield request flag
    
    // Job execution metrics
    uint64_t memory_allocations;            // Number of memory allocations
    uint64_t token_accesses;                // Number of token access operations
    uint64_t policy_validations;            // Number of policy validation checks
} rift_job_context_t;

// =============================================================================
// GOVERNANCE CONTRACT VALIDATION FUNCTIONS
// =============================================================================

/**
 * @brief Initialize a token access envelope with comprehensive validation
 * @param envelope Pointer to envelope structure to initialize
 * @param parent_authority Parent thread authority for inheritance
 * @param policy_name Name of policy to apply
 * @return 0 on success, error code on failure
 */
int rift_envelope_init(rift_token_access_envelope_t* envelope,
                       const rift_thread_authority_t* parent_authority,
                       const char* policy_name);

/**
 * @brief Validate token access against envelope permissions
 * @param envelope Token access envelope to validate against
 * @param memory_address Memory address being accessed
 * @param access_type Type of access being requested (R/W/X flags)
 * @return true if access is permitted, false if denied
 */
bool rift_envelope_validate_access(const rift_token_access_envelope_t* envelope,
                                   void* memory_address,
                                   uint32_t access_type);

/**
 * @brief Create memory segment boundary for governance enforcement
 * @param segment Pointer to segment structure to initialize
 * @param base_address Base memory address for segment
 * @param size Size of memory segment in bytes
 * @param span_type Type of memory span (row/fixed/superposed)
 * @return 0 on success, error code on failure
 */
int rift_segment_create(rift_memory_segment_t* segment,
                        void* base_address,
                        size_t size,
                        rift_span_type_t span_type);

/**
 * @brief Validate thread authority inheritance chain
 * @param authority Authority structure to validate
 * @param parent_context Parent job context for inheritance validation
 * @return true if authority chain is valid, false otherwise
 */
bool rift_authority_validate_chain(const rift_thread_authority_t* authority,
                                   const rift_job_context_t* parent_context);

/**
 * @brief Perform governance checkpoint validation
 * @param job_context Job context to validate
 * @param checkpoint_type Type of checkpoint being performed
 * @return 0 on successful validation, error code on failure
 */
int rift_governance_checkpoint(rift_job_context_t* job_context,
                               const char* checkpoint_type);

/**
 * @brief Calculate policy validation ratio for Gate 1 compliance
 * @param job_context Job context to analyze
 * @return Policy validation ratio as percentage (0.0 to 1.0)
 */
double rift_calculate_policy_validation_ratio(const rift_job_context_t* job_context);

/**
 * @brief Log access violation for governance audit trail
 * @param envelope Token access envelope where violation occurred
 * @param violation_type Description of violation type
 * @param memory_address Address where violation occurred
 */
void rift_envelope_log_violation(rift_token_access_envelope_t* envelope,
                                 const char* violation_type,
                                 void* memory_address);

/**
 * @brief Clean up and destroy token access envelope
 * @param envelope Envelope to clean up
 */
void rift_envelope_destroy(rift_token_access_envelope_t* envelope);

// =============================================================================
// GOVERNANCE CONTRACT ENFORCEMENT MACROS
// =============================================================================

/**
 * @brief Macro for governance checkpoint validation with automatic failure handling
 */
#define RIFT_GOVERNANCE_CHECKPOINT(job_ctx, checkpoint_name) \
    do { \
        int _checkpoint_result = rift_governance_checkpoint(job_ctx, checkpoint_name); \
        if (_checkpoint_result != 0) { \
            fprintf(stderr, "[GOVERNANCE] Checkpoint failed: %s (error %d)\n", \
                    checkpoint_name, _checkpoint_result); \
            return _checkpoint_result; \
        } \
    } while(0)

/**
 * @brief Macro for token access validation with violation logging
 */
#define RIFT_VALIDATE_TOKEN_ACCESS(envelope, addr, access_flags) \
    do { \
        if (!rift_envelope_validate_access(envelope, addr, access_flags)) { \
            rift_envelope_log_violation(envelope, "UNAUTHORIZED_ACCESS", addr); \
            return -1; \
        } \
    } while(0)

/**
 * @brief Macro for cooperative yield with governance state preservation
 */
#define RIFT_COOPERATIVE_YIELD(job_ctx) \
    do { \
        clock_gettime(CLOCK_MONOTONIC, &(job_ctx)->last_yield_time); \
        (job_ctx)->yield_requested = false; \
        pthread_yield(); \
    } while(0)

#endif // TOKEN_ACCESS_ENVELOPE_H
/**
 * RIFT Concurrency Governance - Modular Architecture
 * Aegis Development Team - Waterfall Implementation
 * 
 * Project Structure:
 * rift_concurrency/
 * ��� common/
 * �   ��� rift_common.h           # Shared definitions and structures
 * �   ��� rift_telemetry.c/h      # PID/TID tracking and spawn telemetry
 * �   ��� rift_governance.c/h     # Core governance contracts
 * ��� simulated/
 * �   ��� rift_simulated.c        # Single-thread cooperative multitasking
 * �   ��� rift_simulated.h        # Simulated concurrency interface
 * �   ��� Makefile                # Simulated concurrency build
 * ��� true_concurrency/
 * �   ��� rift_true_concurrency.c # Multi-thread true concurrency
 * �   ��� rift_process_hierarchy.c # Parent-child process management
 * �   ��� rift_true_concurrency.h # True concurrency interface
 * �   ��� Makefile                # True concurrency build
 * ��� tests/
 * �   ��� test_simulated.c        # Simulated concurrency tests
 * �   ��� test_true_concurrency.c # True concurrency tests
 * ��� Makefile.master             # Master build coordination
 */

// =============================================================================
// COMMON DEFINITIONS - rift_common.h
// =============================================================================

#ifndef RIFT_COMMON_H
#define RIFT_COMMON_H


// Maximum hierarchy constraints per RIFT governance
#define RIFT_MAX_CHILDREN_PER_PROCESS 32
#define RIFT_MAX_HIERARCHY_DEPTH 8
#define RIFT_MAX_THREAD_COUNT 256

// Telemetry and tracking structures
typedef struct {
    pid_t process_id;               // System process ID
    pthread_t thread_id;            // Thread ID (if applicable)
    uint64_t rift_thread_id;        // Internal RIFT thread identifier
    pid_t parent_process_id;        // Parent process PID
    uint64_t parent_rift_id;        // Parent RIFT thread ID
    struct timespec spawn_time;     // Thread/process creation timestamp
    char spawn_location[128];       // Where in code this was spawned
    uint32_t hierarchy_depth;       // Depth in parent-child tree
    uint32_t child_count;           // Number of children spawned
    bool is_daemon;                 // Daemon thread flag
} rift_spawn_telemetry_t;

// Governance policy structure (shared between modules)
typedef enum {
    CONCURRENCY_SIMULATED,          // Single-thread cooperative
    CONCURRENCY_TRUE_THREAD,        // Multi-thread within process
    CONCURRENCY_TRUE_PROCESS        // Multi-process hierarchy
} rift_concurrency_mode_t;

typedef enum {
    DESTROY_CASCADE,                // Cascade destruction to children
    DESTROY_KEEP_ALIVE,            // Allow children to survive
    DESTROY_GRACEFUL,              // Graceful shutdown signal
    DESTROY_IMMEDIATE              // Immediate termination
} rift_destroy_policy_t;

typedef struct {
    uint64_t rift_id;              // Internal RIFT identifier
    rift_concurrency_mode_t mode;  // Concurrency execution mode
    rift_destroy_policy_t destroy_policy; // Child destruction policy
    uint32_t max_children;         // Maximum children allowed
    uint32_t max_execution_time_ms; // Execution time limit
    bool trace_capped;             // Enable hierarchy depth limits
    uint32_t max_hierarchy_depth;  // Maximum tree depth
    bool daemon_mode;              // Daemon thread flag
    bool keep_alive;               // Survival policy flag
} rift_governance_policy_t;

// Thread context structure (shared between modules)
typedef struct {
    rift_spawn_telemetry_t telemetry;     // Spawn tracking and telemetry
    rift_governance_policy_t policy;      // Governance and policy constraints
    struct timespec last_heartbeat;       // Last activity timestamp
    uint32_t context_switches;            // Context switch counter
    volatile bool should_terminate;       // Termination signal
    void* module_specific_data;           // Module-specific context
} rift_thread_context_t;

// Memory token for resource governance
typedef struct {
    uint64_t token_id;             // Unique token identifier
    uint64_t owner_rift_id;        // Owning RIFT thread ID
    pid_t owner_process_id;        // Owning process ID
    uint32_t access_mask;          // Permission bit mask (R/W/X)
    char resource_name[64];        // Resource identifier
    struct timespec acquisition_time; // When token was acquired
    uint32_t validation_bits;      // Token state validation
    bool is_transferable;          // Can be transferred between threads
} rift_memory_token_t;

#endif // RIFT_COMMON_H

// =============================================================================
// TELEMETRY IMPLEMENTATION - rift_telemetry.h
// =============================================================================

#ifndef RIFT_TELEMETRY_H
#define RIFT_TELEMETRY_H

/* RIFT_CLEANUP: Commented out missing header
 * #include "rift_common.h"
 */

/**
 * @brief Initialize telemetry subsystem
 * @return 0 on success, error code otherwise
 */
int rift_telemetry_init(void);

/**
 * @brief Register new thread/process spawn with telemetry
 * @param context Thread context to register
 * @param spawn_location Source location where spawn occurred
 * @return 0 on success, error code otherwise
 */
int rift_telemetry_register_spawn(rift_thread_context_t* context, const char* spawn_location);

/**
 * @brief Update heartbeat for thread/process
 * @param rift_id RIFT thread identifier
 * @return 0 on success, error code otherwise
 */
int rift_telemetry_heartbeat(uint64_t rift_id);

/**
 * @brief Get spawn telemetry for specific thread
 * @param rift_id RIFT thread identifier
 * @return Pointer to telemetry data, NULL if not found
 */
rift_spawn_telemetry_t* rift_telemetry_get(uint64_t rift_id);

/**
 * @brief Print comprehensive telemetry report
 */
void rift_telemetry_print_report(void);

/**
 * @brief Validate hierarchy constraints for new spawn
 * @param parent_rift_id Parent RIFT thread ID
 * @param proposed_policy Proposed governance policy
 * @return true if spawn allowed, false if violates constraints
 */
bool rift_telemetry_validate_spawn(uint64_t parent_rift_id, const rift_governance_policy_t* proposed_policy);

#endif // RIFT_TELEMETRY_H
