#!/bin/bash
# Fix RIFT-0 context structure definitions for DSL compilation
# Part of RIFT DSL development - yacc toolchain competitor

echo "=== RIFT DSL Context Structure Resolution ==="
echo "Fixing RiftStage0Context and related structures..."

# Update rift-0.h with complete structure definitions
cat > include/rift-0/core/rift-0.h << 'EOF'
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
EOF

# Update tokenizer_types.h to include missing token types
cat >> include/rift-0/core/lexer/tokenizer_types.h << 'EOF'

/* Additional token types for DSL processing */
#define TOKEN_DELIMITER TOKEN_OPERATOR  /* Temporary alias until proper implementation */
#define TOKEN_R_PATTERN TOKEN_LITERAL_STRING  /* R-pattern tokens */

/* DSL-specific token categories */
typedef enum {
    TOKEN_DSL_RULE = TOKEN_TYPE_COUNT,
    TOKEN_DSL_ACTION,
    TOKEN_DSL_DIRECTIVE,
    TOKEN_DSL_PATTERN,
    TOKEN_DSL_COUNT
} DSLTokenType;

EOF

# Create a minimal rift-0.c implementation fix
cat > src/core/rift-0-fix.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "rift-0/core/rift-0.h"
#include "rift-0/core/lexer/tokenizer.h"

/* Version information for DSL */
static const char* RIFT_VERSION = "0.1.0-dsl";

/**
 * Create RIFT Stage 0 context for DSL processing
 */
RiftStage0Context* rift_stage0_create(void) {
    RiftStage0Context* ctx = calloc(1, sizeof(RiftStage0Context));
    if (!ctx) {
        return NULL;
    }
    
    /* Initialize tokenizer for DSL parsing */
    ctx->tokenizer = rift_tokenizer_create();
    if (!ctx->tokenizer) {
        free(ctx);
        return NULL;
    }
    
    /* Initialize memory governor */
    ctx->mem_gov = calloc(1, sizeof(RiftMemoryGovernor));
    if (ctx->mem_gov) {
        pthread_mutex_init(&ctx->mem_gov->mem_lock, NULL);
    }
    
    /* Initialize context lock */
    pthread_mutex_init(&ctx->ctx_lock, NULL);
    
    /* Set defaults */
    ctx->debug_mode = false;
    ctx->strict_mode = true;  /* Strict mode for DSL compilation */
    
    return ctx;
}

/**
 * Destroy RIFT context and free resources
 */
void rift_stage0_destroy(RiftStage0Context* ctx) {
    if (!ctx) return;
    
    if (ctx->tokenizer) {
        rift_tokenizer_destroy(ctx->tokenizer);
    }
    
    if (ctx->mem_gov) {
        pthread_mutex_destroy(&ctx->mem_gov->mem_lock);
        free(ctx->mem_gov);
    }
    
    pthread_mutex_destroy(&ctx->ctx_lock);
    free(ctx);
}

/**
 * Process RIFT build script (DSL input)
 */
int rift_process_build_script(RiftStage0Context* ctx, const char* script) {
    if (!ctx || !script) {
        return -1;
    }
    
    pthread_mutex_lock(&ctx->ctx_lock);
    
    /* Reset error state */
    ctx->has_error = false;
    ctx->error_message[0] = '\0';
    
    /* TODO: Implement DSL script processing */
    /* This will parse RIFT DSL syntax and generate build rules */
    
    pthread_mutex_unlock(&ctx->ctx_lock);
    
    return 0;
}

/**
 * Compile a RIFT pattern (similar to yacc rules)
 */
int rift_compile_pattern(RiftStage0Context* ctx, const char* pattern) {
    if (!ctx || !pattern) {
        return -1;
    }
    
    /* TODO: Implement pattern compilation for DSL */
    /* This competes with yacc's pattern matching */
    
    ctx->stats.patterns_compiled++;
    
    return 0;
}

/**
 * Generate parser from RIFT DSL (yacc competitor feature)
 */
int rift_generate_parser(RiftStage0Context* ctx, const char* output_file) {
    if (!ctx || !output_file) {
        return -1;
    }
    
    /* TODO: Generate C parser from RIFT DSL rules */
    /* This is the core feature competing with yacc */
    
    return 0;
}

/**
 * Tokenize input for DSL processing
 */
int rift_tokenize_input(RiftStage0Context* ctx, const char* input, 
                       TokenTriplet* tokens, size_t max_tokens) {
    if (!ctx || !input || !tokens || max_tokens == 0) {
        return -1;
    }
    
    pthread_mutex_lock(&ctx->ctx_lock);
    
    /* Use tokenizer to process DSL input */
    size_t token_count = 0;
    ssize_t result = rift_tokenizer_process_with_flags(
        ctx->tokenizer, 
        input, 
        strlen(input),
        TOKEN_FLAG_NONE
    );
    
    if (result >= 0) {
        token_count = rift_tokenizer_get_tokens(
            ctx->tokenizer,
            tokens,
            max_tokens
        );
        ctx->stats.tokens_processed += token_count;
    }
    
    pthread_mutex_unlock(&ctx->ctx_lock);
    
    return (result >= 0) ? (int)token_count : -1;
}

/**
 * Print RIFT DSL statistics
 */
void rift_print_statistics(const RiftStage0Context* ctx) {
    if (!ctx) return;
    
    printf("RIFT DSL Statistics:\n");
    printf("  Tokens Processed: %zu\n", ctx->stats.tokens_processed);
    printf("  Patterns Compiled: %zu\n", ctx->stats.patterns_compiled);
    printf("  Memory Allocated: %zu bytes\n", ctx->stats.memory_allocated);
    printf("  DFA States: %zu\n", ctx->stats.dfa_states_created);
    printf("  Processing Time: %.3f seconds\n", ctx->stats.processing_time);
    
    if (ctx->mem_gov) {
        printf("  Peak Memory: %zu bytes\n", ctx->mem_gov->peak_usage);
        printf("  Allocations: %zu\n", ctx->mem_gov->allocation_count);
    }
}

/**
 * Get RIFT version
 */
const char* rift_get_version(void) {
    return RIFT_VERSION;
}

/* Stub for tokenize_input to resolve linking */
int tokenize_input(const char* input, void* tokens, size_t max_tokens) {
    /* This is a compatibility stub */
    return -1;
}
EOF

# Replace the problematic rift-0.c with our fixed version
mv src/core/rift-0.c src/core/rift-0.c.backup
cp src/core/rift-0-fix.c src/core/rift-0.c

echo "=== Context structure fixes applied ==="
echo "RIFT DSL development can now proceed with yacc-competitive features"
