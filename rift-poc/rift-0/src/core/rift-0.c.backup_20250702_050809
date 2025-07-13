/*
 * RIFT-Core-0: Stage-0 Orchestration (Tokenization Only)
 * Part of AEGIS Project - OBINexus Computing
 *
 * This file now only handles Stage-0 orchestration and test main.
 * Lexical analysis logic is in src/core/lexer.c (see include/rift-0/lexer.h)
 * Governance: #[gov:stage_0_core]
 */

// Only standard includes needed for main/test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <malloc.h>
#else
#include <strings.h> // For strdup on some systems
#endif
#include <regex.h>
#include <pthread.h>
#include "rift-0/core/rift-0.h"
#include "rift-0/core/gov/rift-gov.0.h"

/* ===================================================================
 * Stage-0 Core Implementation
 * =================================================================== */

RiftStage0Context* rift_stage0_create(void) {
    RiftStage0Context* ctx = calloc(1, sizeof(RiftStage0Context));
    if (!ctx) return NULL;
    ctx->mem_gov = create_memory_governor(1024 * 1024, 16 * 1024 * 1024); // 1MB min, 16MB max
    if (!ctx->mem_gov) {
        free(ctx);
        return NULL;
    }
    // Use a fixed pattern count or define stage0_patterns in one place only
    extern size_t stage0_patterns_count; // Declare this in a header and define it where stage0_patterns is defined
    ctx->pattern_count = stage0_patterns_count;
    ctx->patterns = calloc(ctx->pattern_count, sizeof(regex_t));
    if (!ctx->patterns) {
        free(ctx->mem_gov);
        free(ctx);
        return NULL;
    }
    
    /* Compile regex patterns */
    for (size_t i = 0; i < ctx->pattern_count; i++) {
        int ret = regcomp(&ctx->patterns[i], stage0_patterns[i].pattern, REG_EXTENDED);
        if (ret != 0) {
            printf("Failed to compile pattern: %s\n", stage0_patterns[i].name);
            // Cleanup and return
            for (size_t j = 0; j < i; j++) {
                regfree(&ctx->patterns[j]);
            }
            free(ctx->patterns);
            free(ctx->mem_gov);
            free(ctx);
            return NULL;
        }
    }
    
    /* Enable dual-channel mode by default */
    ctx->dual_mode_enabled = true;
    ctx->quantum_mode_active = false;
    ctx->aegis_compliant = true;
    ctx->compliance_flags = 0xAE015;
    ctx->thread_count = 4;
    
    pthread_mutex_init(&ctx->ctx_lock, NULL);
    
    printf("RIFT Stage-0 Tokenizer initialized\n");
    printf("  Version: %d.%d.%d\n", RIFT_VERSION_MAJOR, RIFT_VERSION_MINOR, RIFT_VERSION_PATCH);
    printf("  Dual-channel: %s\n", ctx->dual_mode_enabled ? "enabled" : "disabled");
    printf("  AEGIS compliant: %s\n", ctx->aegis_compliant ? "yes" : "no");
    printf("  Memory limits: %zu - %zu bytes\n", ctx->mem_gov->min_heap, ctx->mem_gov->max_heap);
    
    return ctx;
    token->type = type;
    token->value = strdup(value);
    token->line = line;
    token->column = col;
    token->is_quantum = false;
    token->is_collapsed = false;
    // Set governance_flags from context, or default to 0 if unavailable
    token->governance_flags = ctx ? ctx->compliance_flags : 0;
    token->metadata = NULL;
    token->pattern = NULL;

    /* Set quantum flag based on token type */
    if (type == R_QUANTUM_TOKEN || type == R_COLLAPSE_MARKER || type == R_ENTANGLE_MARKER) {
        token->is_quantum = true;
    }

    return token;
}
    // Set governance_flags from context, or default to 0 if unavailable
    token->governance_flags = ctx ? ctx->compliance_flags : 0;
    token->metadata = NULL;
    token->pattern = NULL;
    
    /* Set quantum flag based on token type */
    if (type == R_QUANTUM_TOKEN || type == R_COLLAPSE_MARKER || type == R_ENTANGLE_MARKER) {
        token->is_quantum = true;
    }
    
    return token;
}


void free_token(RiftStage0Context* ctx, RiftToken* token) {
    if (!ctx || !token) return;
    
    size_t token_size = sizeof(RiftToken) + (token->value ? strlen(token->value) + 1 : 0);
    
    if (token->value) free(token->value);
    if (token->pattern) free(token->pattern);
    if (token->metadata) free(token->metadata);
    
    governed_free(ctx->mem_gov, token, token_size);
}

/* ===================================================================
 * Tokenization Engine
 * =================================================================== */

RiftToken** tokenize_input(RiftStage0Context* ctx, const char* input, size_t* token_count) {
    if (!ctx || !input || !token_count) return NULL;
    
    pthread_mutex_lock(&ctx->ctx_lock);
    
    /* Initial token array */
    size_t capacity = 100;
    RiftToken** tokens = calloc(capacity, sizeof(RiftToken*));
    if (!tokens) {
        pthread_mutex_unlock(&ctx->ctx_lock);
        return NULL;
    }
    
    size_t count = 0;
    size_t line = 1;
    size_t col = 1;
    const char* ptr = input;
    
    while (*ptr) {
        bool matched = false;
        
        /* Skip whitespace while tracking position */
        if (*ptr == ' ' || *ptr == '\t') {
            col++;
            regmatch_t match;
            char test_buffer[256];

            /* Extract a test string */
            size_t test_len = 0;
            const char* test_ptr = ptr;
            while (*test_ptr && *test_ptr != ' ' && *test_ptr != '\n' &&
                   *test_ptr != '\t' && test_len < 255) {
                test_buffer[test_len++] = *test_ptr++;
            }
            test_buffer[test_len] = '\0';

            // Use the correct field for token type in stage0_patterns
            // If your TokenPattern struct does not have 'type', replace with the correct field (e.g., .token_type)
            RiftTokenType pattern_type = stage0_patterns[i].type;
            if (test_len > 0 && regexec(&ctx->patterns[i], test_buffer, 1, &match, 0) == 0) {
                /* Create token */
                RiftToken* token = create_token(ctx, pattern_type,
                                              test_buffer, line, col);
            size_t test_len = 0;
            const char* test_ptr = ptr;
            while (*test_ptr && *test_ptr != ' ' && *test_ptr != '\n' && 
                   *test_ptr != '\t' && test_len < 255) {
                RiftToken* token = create_token(ctx, stage0_patterns[i].type,
                                              test_buffer, line, col);
            test_buffer[test_len] = '\0';
            
            if (test_len > 0 && regexec(&ctx->patterns[i], test_buffer, 1, &match, 0) == 0) {
                /* Create token */
                RiftToken* token = create_token(ctx, stage0_patterns[i].type, 
                                              test_buffer, line, col);
                if (!token) {
                    /* Memory error */
                    DualChannelOutput* error_output = create_dual_channel_output();
                    set_error_level(error_output, RIFT_CRITICAL_MIN, 
                                  "Memory allocation failed during tokenization");
                    pthread_mutex_unlock(&ctx->ctx_lock);
                    return NULL;
                }
                
                /* Check for quantum mode macro */
                if (strcmp(test_buffer, "!quantum") == 0) {
                    ctx->quantum_mode_active = true;
                    printf("Entering quantum mode\n");
                } else if (strcmp(test_buffer, "!classic") == 0) {
                    ctx->quantum_mode_active = false;
                    printf("Entering classic mode\n");
                }
                
                /* Expand array if needed */
                if (count >= capacity) {
                    capacity *= 2;
                    RiftToken** new_tokens = realloc(tokens, capacity * sizeof(RiftToken*));
                    if (!new_tokens) {
                        pthread_mutex_unlock(&ctx->ctx_lock);
                        return NULL;
                    }
                    tokens = new_tokens;
                }
                
                tokens[count++] = token;
                ptr += test_len;
                col += test_len;
                matched = true;
                break;
            }
        }
        
        if (!matched) {
            /* Unknown token - generate error */
            char err_msg[256];
            snprintf(err_msg, sizeof(err_msg), 
                    "Unknown token at line %zu, column %zu: '%c'", line, col, *ptr);
            
            DualChannelOutput* error_output = create_dual_channel_output();
            set_error_level(error_output, RIFT_WARNING_MAX - 1, err_msg);
            
            ptr++;
            col++;
        }
    }
    
    /* Add EOF token */
    RiftToken* eof = create_token(ctx, R_EOF, "EOF", line, col);
    if (eof) {
        if (count >= capacity) {
            capacity++;
            RiftToken** new_tokens = realloc(tokens, capacity * sizeof(RiftToken*));
            if (new_tokens) {
                tokens = new_tokens;
            }
        }
        tokens[count++] = eof;
    }
    
    *token_count = count;
    pthread_mutex_unlock(&ctx->ctx_lock);
    
    return tokens;
}

/* ===================================================================
 * Dual-Channel Processing
 * =================================================================== */

DualChannelOutput* process_stage0(RiftStage0Context* ctx, const char* input) {
    if (!ctx || !input) return NULL;
    
    DualChannelOutput* output = create_dual_channel_output();
    if (!output) return NULL;
    
    size_t token_count = 0;
    RiftToken** tokens = tokenize_input(ctx, input, &token_count);
    if (!tokens) {
        set_error_level(output, RIFT_CRITICAL_MIN, "Tokenization failed");
        return output;
    }
    
    /* Allocate channels */
    size_t classic_capacity = 4096;
    size_t quantum_capacity = 4096;
    char* classic_buffer = calloc(classic_capacity, 1);
    char* quantum_buffer = calloc(quantum_capacity, 1);
    
    if (!classic_buffer || !quantum_buffer) {
        set_error_level(output, RIFT_CRITICAL_MIN, "Channel allocation failed");
        return output;
    }
    
    size_t classic_offset = 0;
    size_t quantum_offset = 0;
    
    /* Process tokens into dual channels */
    for (size_t i = 0; i < token_count; i++) {
        RiftToken* token = tokens[i];
        char token_str[512];
        
        snprintf(token_str, sizeof(token_str), 
                "Token[%zu]: type=%d, value='%s', line=%zu, col=%zu, quantum=%s\n",
                i, token->type, token->value, token->line, token->column,
                token->is_quantum ? "true" : "false");
        
        if (token->is_quantum || ctx->quantum_mode_active) {
            /* Route to quantum channel */
            size_t len = strlen(token_str);
            if (quantum_offset + len >= quantum_capacity) {
                quantum_capacity *= 2;
                char* new_buffer = realloc(quantum_buffer, quantum_capacity);
                if (!new_buffer) {
                    set_error_level(output, RIFT_DANGER_MIN, "Quantum channel overflow");
                    break;
                }
                quantum_buffer = new_buffer;
            }
            strcpy(quantum_buffer + quantum_offset, token_str);
            quantum_offset += len;
        } else {
            /* Route to classic channel */
            size_t len = strlen(token_str);
            if (classic_offset + len >= classic_capacity) {
                classic_capacity *= 2;
                char* new_buffer = realloc(classic_buffer, classic_capacity);
                if (!new_buffer) {
                    set_error_level(output, RIFT_DANGER_MIN, "Classic channel overflow");
                    break;
                }
                classic_buffer = new_buffer;
            }
            strcpy(classic_buffer + classic_offset, token_str);
            classic_offset += len;
        }
    }
    
    /* Set output channels */
    output->classic_channel = classic_buffer;
    output->classic_size = classic_offset;
    output->quantum_channel = quantum_buffer;
    output->quantum_size = quantum_offset;
    
    /* Add metadata */
    char metadata[1024];
    snprintf(metadata, sizeof(metadata),
            "\n# RIFT Stage-0 Metadata\n"
            "# Version: %d.%d.%d\n"
            "# Tokens: %zu\n"
            "# Classic channel: %zu bytes\n"
            "# Quantum channel: %zu bytes\n"
            "# Memory usage: %zu/%zu bytes\n"
            "# AEGIS compliant: %s\n"
            "# Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n",
            RIFT_VERSION_MAJOR, RIFT_VERSION_MINOR, RIFT_VERSION_PATCH,
            token_count, classic_offset, quantum_offset,
            ctx->mem_gov->current_usage, ctx->mem_gov->max_heap,
            ctx->aegis_compliant ? "yes" : "no");
    
    /* Append metadata to classic channel */
    size_t meta_len = strlen(metadata);
    if (classic_offset + meta_len < classic_capacity) {
        strcpy(classic_buffer + classic_offset, metadata);
        output->classic_size += meta_len;
    }
    
    /* Cleanup tokens */
    for (size_t i = 0; i < token_count; i++) {
        free_token(ctx, tokens[i]);
    }
    free(tokens);
    
    printf("Stage-0 processing complete\n");
    printf("  Classic channel: %zu bytes\n", output->classic_size);
    printf("  Quantum channel: %zu bytes\n", output->quantum_size);
    printf("  Error level: %d\n", output->error_level);
    
    return output;
}

/* ===================================================================
 * Build Output Generation
 * =================================================================== */

// BuildOutput is typedef'd in rift-0.h, do not redefine here.
    build->obj_path = strdup("build/obj/rift-stage0.o");
    build->bin_path = strdup("build/bin/rift-stage0");
    build->lib_path = strdup("build/lib/librift-stage0.so");
    build->build_success = true;

    printf("Build output generated:\n");
    printf("  Object: %s\n", build->obj_path);
    printf("  Binary: %s\n", build->bin_path);
    printf("  Library: %s\n", build->lib_path);

    return build;
}
    }
    
    /* Generate paths */
    build->obj_path = strdup("build/obj/rift-stage0.o");
    build->bin_path = strdup("build/bin/rift-stage0");
    build->lib_path = strdup("build/lib/librift-stage0.so");
    build->build_success = true;
    
    printf("Build output generated:\n");
    printf("  Object: %s\n", build->obj_path);
    printf("  Binary: %s\n", build->bin_path);
    printf("  Library: %s\n", build->lib_path);
    
    return build;
}

/* ===================================================================
 * Cleanup
 * =================================================================== */

void rift_stage0_destroy(RiftStage0Context* ctx) {
    if (!ctx) return;
    
    // Example: Use stage queue for stage-bound cleanup (expand as needed)
    RiftStageQueue stage_queue;
    rift_stage_queue_init(&stage_queue, 8);
    rift_stage_tracker_load_from_xml(&stage_queue, "gov.riftrc.0.in.xml");
    while (!rift_stage_queue_empty(&stage_queue)) {
        RiftStageEntry entry = rift_stage_queue_pop(&stage_queue);
        if (entry.active) {
            // For demonstration, only handle tokenization stage cleanup
            if (strcmp(entry.name, "tokenization") == 0) {
                for (size_t i = 0; i < ctx->pattern_count; i++) {
                    regfree(&ctx->patterns[i]);
                }
                free(ctx->patterns);
            }
            // Add more stage-specific cleanup as needed
        }
    }
    rift_stage_queue_free(&stage_queue);

    /* Free memory governor */
    if (ctx->mem_gov) {
        pthread_mutex_destroy(&ctx->mem_gov->mem_lock);
        free(ctx->mem_gov);
    }

    pthread_mutex_destroy(&ctx->ctx_lock);
    free(ctx);
}


