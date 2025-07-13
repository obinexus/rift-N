
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rift-0/core/rift-0.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "rift-0/core/lexer/lexer.h"      // Lexer API, RiftStage0Context, RiftTokenType, etc.
#include "rift-0/core/gov/rift-gov.0.h"   // Governance types and macros
#include "rift-0/core/ext/r_uml.h"        // UML types and commands (uml_relationship_t, parse_uml_relationship, etc.)
#include <stdint.h>


/*
 * RIFT-Core-0: Stage-0 Tokenization and Lexical Analysis
 * Part of AEGIS Project - OBINexus Computing
 * 
 * Implements dual-channel output (classic/quantum) with AEGIS compliance
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 */

static TokenPattern stage0_patterns[] = {
    {"identifier", "^[a-zA-Z_][a-zA-Z0-9_]*$", R_IDENTIFIER, false},
    {"number", "^[0-9]+(\\.[0-9]+)?$", R_NUMBER, false},
    {"operator", "^[+\\-*/=<>!&|^~%]$", R_OPERATOR, false},
    {"quantum_init", "^@quantum\\s*\\{", R_QUANTUM_TOKEN, true},
    {"collapse", "^!collapse", R_COLLAPSE_MARKER, true},
    {"entangle", "^@entangle", R_ENTANGLE_MARKER, true},
    {"governance", "^#\\[gov:.*\\]", R_GOVERNANCE_TAG, false},
    {"string", "^\"([^\"\\\\]|\\\\.)*\"$", R_STRING, false},
    {"comment", "^//.*$", R_COMMENT, false},
    {"whitespace", "^\\s+$", R_WHITESPACE, false}
};

/* ===================================================================
 * Memory Governance Implementation
 * =================================================================== */

TokenMemoryGovernor* create_memory_governor(size_t min_heap, size_t max_heap) {
    TokenMemoryGovernor* gov = malloc(sizeof(TokenMemoryGovernor));
    if (!gov) return NULL;
    
    gov->min_heap = min_heap;
    gov->max_heap = max_heap;
    gov->current_usage = 0;
    gov->dynamic_allowed = true;
    pthread_mutex_init(&gov->mem_lock, NULL);
    
    return gov;
}

void* governed_malloc(TokenMemoryGovernor* gov, size_t size) {
    if (!gov || !gov->dynamic_allowed) return NULL;
    
    pthread_mutex_lock(&gov->mem_lock);
    
    if (gov->current_usage + size > gov->max_heap) {
        pthread_mutex_unlock(&gov->mem_lock);
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (ptr) {
        gov->current_usage += size;
    }
    
    pthread_mutex_unlock(&gov->mem_lock);
    return ptr;
}

void governed_free(TokenMemoryGovernor* gov, void* ptr, size_t size) {
    if (!gov || !ptr) return;
    
    pthread_mutex_lock(&gov->mem_lock);
    free(ptr);
    gov->current_usage -= size;
    pthread_mutex_unlock(&gov->mem_lock);
}

/* ===================================================================
 * Dual-Channel Output Management
 * =================================================================== */

DualChannelOutput* create_dual_channel_output(void) {
    DualChannelOutput* output = calloc(1, sizeof(DualChannelOutput));
    if (!output) return NULL;
    
    output->error_level = 0;
    return output;
}

void set_error_level(DualChannelOutput* output, uint8_t level, const char* msg) {
    if (!output) return;
    
    output->error_level = level;
    if (msg) {
        output->error_msg = strdup(msg);
    }
    
    /* Handle error severity */
    if (level >= RIFT_WARNING_MIN && level < RIFT_WARNING_MAX) {
        printf("\033[33m[WARNING]\033[0m %s\n", msg);
    } else if (level >= RIFT_DANGER_MIN && level < RIFT_DANGER_MAX) {
        printf("\033[31m[DANGER]\033[0m %s - Entering fix mode\n", msg);
    } else if (level >= RIFT_CRITICAL_MIN && level < RIFT_CRITICAL_MAX) {
        printf("\033[35m[CRITICAL]\033[0m %s - Emergency intervention required\n", msg);
    } else if (level >= RIFT_PANIC_MIN) {
        printf("\033[91m[PANIC]\033[0m %s - System failsafe activated\n", msg);
    }
}


void free_dual_channel_output(DualChannelOutput* output) {
    if (!output) return;
    
    if (output->classic_channel) free(output->classic_channel);
    if (output->quantum_channel) free(output->quantum_channel);
    if (output->error_msg) free(output->error_msg); // If error_msg is not present, change to error_message or remove this line
    free(output);
}

void free_build_output(BuildOutput* build) {
    if (!build) return;
    
    if (build->obj_path) free(build->obj_path);
    if (build->bin_path) free(build->bin_path);
    if (build->lib_path) free(build->lib_path);
    free(build);
}

void print_usage() {
    printf("RIFT Stage-0 CLI\n");
    printf("Usage: riftlang [command] [args]\n");
    printf("Commands:\n");
    printf("  token-type <input>      Analyze token types\n");
    printf("  token-mem <input>       Analyze token memory\n");
    printf("  token-value <input>     Analyze token values\n");
    printf("  uml-parse <pattern> <source>   Parse UML relationship\n");
    printf("  uml-validate <pattern> <source> Validate UML governance\n");
    printf("  uml-generate <pattern> <source> Generate UML code\n");
    printf("  (no command)            Run Stage-0 tokenizer on stdin\n");
    printf("  --help                  Show this help message\n");
}
/**
 * @brief Main validation entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit code (0 = success, non-zero = failure)
 */

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 0;
    }

    // If a CLI command is given, handle it; otherwise, process stdin as input
    if (strcmp(argv[1], "token-type") == 0 && argc >= 3) {
        // TODO: Call token type analytics (stub)
        printf("[token-type] Not yet implemented. Input: %s\n", argv[2]);
        return 0;
    } else if (strcmp(argv[1], "token-mem") == 0 && argc >= 3) {
        // TODO: Call token memory analytics (stub)
        printf("[token-mem] Not yet implemented. Input: %s\n", argv[2]);
        return 0;
    } else if (strcmp(argv[1], "token-value") == 0 && argc >= 3) {
        // TODO: Call token value analytics (stub)
        printf("[token-value] Not yet implemented. Input: %s\n", argv[2]);
        return 0;
    } else if (strcmp(argv[1], "uml-parse") == 0 && argc >= 4) {
        uml_relationship_t* rel = parse_uml_relationship(argv[2], argv[3]);
        if (rel) {
            printf("UML relationship parsed successfully.\n");
            // TODO: Print rel fields
            free(rel);
        } else {
            printf("Failed to parse UML relationship.\n");
        }
        return 0;
    } else if (strcmp(argv[1], "uml-validate") == 0 && argc >= 4) {
        uml_relationship_t* rel = parse_uml_relationship(argv[2], argv[3]);
        if (rel) {
            bool valid = validate_uml_governance(rel);
            printf("UML governance %s.\n", valid ? "valid" : "invalid");
            free(rel);
        } else {
            printf("Failed to parse UML relationship.\n");
        }
        return 0;
    } else if (strcmp(argv[1], "uml-generate") == 0 && argc >= 4) {
        uml_relationship_t* rel = parse_uml_relationship(argv[2], argv[3]);
        if (rel) {
            char buffer[1024];
            generate_uml_code(rel, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            free(rel);
        } else {
            printf("Failed to parse UML relationship.\n");
        }
        return 0;
    }

    // No command: process stdin as input for Stage-0
    RiftStage0Context* ctx = rift_stage0_create();
    if (!ctx) {
        fprintf(stderr, "Failed to initialize Stage-0 context\n");
        return 1;
    }

    // Read all of stdin into a buffer
    char* input = NULL;
    size_t input_size = 0;
    size_t input_cap = 4096;
    input = malloc(input_cap);
    if (!input) {
        fprintf(stderr, "Failed to allocate input buffer\n");
        rift_stage0_destroy(ctx);
        return 1;
    }
    size_t len = 0;
    int c;
    while ((c = fgetc(stdin)) != EOF) {
        if (len + 1 >= input_cap) {
            input_cap *= 2;
            char* new_input = realloc(input, input_cap);
            if (!new_input) {
                fprintf(stderr, "Failed to reallocate input buffer\n");
                free(input);
                rift_stage0_destroy(ctx);
                return 1;
            }
            input = new_input;
        }
        input[len++] = (char)c;
    }
    input[len] = '\0';

    DualChannelOutput* output = process_stage0(ctx, input);
    if (!output) {
        fprintf(stderr, "Stage-0 processing failed\n");
        free(input);
        rift_stage0_destroy(ctx);
        return 1;
    }

    // Output classic channel to stdout, quantum channel to stderr
    if (output->classic_channel) {
        fwrite(output->classic_channel, 1, output->classic_size, stdout);
    }
    if (output->quantum_channel && output->quantum_size > 0) {
        fwrite(output->quantum_channel, 1, output->quantum_size, stderr);
    }

    free_dual_channel_output(output);
    free(input);
    rift_stage0_destroy(ctx);
    return 0;
}

