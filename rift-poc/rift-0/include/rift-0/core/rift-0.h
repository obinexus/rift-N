#ifndef RIFT_0_H
#define RIFT_0_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "rift-0/core/lexer/tokenizer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
typedef struct RiftStage0Context RiftStage0Context;
typedef struct RiftStats RiftStats;
typedef struct RiftMemoryGovernor RiftMemoryGovernor;

/* RIFT Statistics tracking */
struct RiftStats {
    size_t tokens_processed;
    size_t patterns_compiled;
    size_t memory_allocated;
    size_t dfa_states_created;
    double processing_time;
};

/* Memory governance for zero-trust architecture */
struct RiftMemoryGovernor {
    size_t total_allocated;
    size_t peak_usage;
    size_t allocation_count;
    pthread_mutex_t mem_lock;
};

/* RIFT Stage 0 DSL Context */
struct RiftStage0Context {
    /* Tokenizer subsystem */
    TokenizerContext* tokenizer;
    
    /* Memory governance */
    RiftMemoryGovernor* mem_gov;
    
    /* Statistics tracking */
    RiftStats stats;
    
    /* Thread safety */
    pthread_mutex_t ctx_lock;
    
    /* Configuration */
    bool debug_mode;
    bool strict_mode;
    
    /* Error handling */
    bool has_error;
    char error_message[512];
};

/* RIFT DSL API - Core functions for build language processing */
RiftStage0Context* rift_stage0_create(void);
void rift_stage0_destroy(RiftStage0Context* ctx);

/* DSL Processing functions */
int rift_process_build_script(RiftStage0Context* ctx, const char* script);
int rift_compile_pattern(RiftStage0Context* ctx, const char* pattern);
int rift_generate_parser(RiftStage0Context* ctx, const char* output_file);

/* Token processing for DSL */
int rift_tokenize_input(RiftStage0Context* ctx, const char* input, 
                       TokenTriplet* tokens, size_t max_tokens);

/* Statistics and debugging */
void rift_print_statistics(const RiftStage0Context* ctx);
const char* rift_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_H */
