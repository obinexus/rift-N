/*
 * =================================================================
 * rift_tb_parser.c - RIFT Dual-Mode Parser Implementation
 * RIFT: RIFT Is a Flexible Translator  
 * Component: Thread-safe dual parsing with parity elimination
 * OBINexus Computing Framework - AEGIS Compliant
 * 
 * Implements: tbtbbt pattern execution with 101001 parity
 * =================================================================
 */

#define RIFT_TB_IMPLEMENTATION
#include "rift_tb_parser.h"

/* RIFT_CLEANUP: Forward declarations to resolve type issues */
#ifndef RIFT_CLEANUP_FORWARD_DECLS
#define RIFT_CLEANUP_FORWARD_DECLS

/* Forward declare missing types if not already defined */
#ifndef RIFT_TOKEN_TYPE_DEFINED
#define RIFT_TOKEN_TYPE_DEFINED
typedef enum TokenType RiftTokenType;
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


#include <errno.h>
#include <fcntl.h>
#include <assert.h>

/* =================================================================
 * PRIORITY QUEUE FOR TOP-DOWN SCHEDULING
 * =================================================================
 */

typedef struct {
    void* data;
    int priority;
} PQNode;

typedef struct {
    PQNode* nodes;
    size_t size;
    size_t capacity;
} PriorityQueue;

static PriorityQueue* pq_create(size_t capacity) {
    PriorityQueue* pq = calloc(1, sizeof(PriorityQueue));
    if (!pq) return NULL;
    
    pq->nodes = calloc(capacity, sizeof(PQNode));
    if (!pq->nodes) {
        free(pq);
        return NULL;
    }
    
    pq->capacity = capacity;
    pq->size = 0;
    return pq;
}

static void pq_heapify_up(PriorityQueue* pq, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (pq->nodes[parent].priority >= pq->nodes[idx].priority) break;
        
        /* Swap with parent */
        PQNode temp = pq->nodes[parent];
        pq->nodes[parent] = pq->nodes[idx];
        pq->nodes[idx] = temp;
        idx = parent;
    }
}

static bool pq_push(PriorityQueue* pq, void* data, int priority) {
    if (!pq || pq->size >= pq->capacity) return false;
    
    pq->nodes[pq->size].data = data;
    pq->nodes[pq->size].priority = priority;
    pq_heapify_up(pq, pq->size);
    pq->size++;
    return true;
}

/* =================================================================
 * STACK IMPLEMENTATION FOR PARSING
 * =================================================================
 */

typedef struct StackNode {
    void* data;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top;
    size_t size;
    pthread_mutex_t mutex;
} ParseStack;

static ParseStack* stack_create(void) {
    ParseStack* stack = calloc(1, sizeof(ParseStack));
    if (!stack) return NULL;
    
    pthread_mutex_init(&stack->mutex, NULL);
    return stack;
}

static bool stack_push(ParseStack* stack, void* data) {
    if (!stack) return false;
    
    pthread_mutex_lock(&stack->mutex);
    
    StackNode* node = calloc(1, sizeof(StackNode));
    if (!node) {
        pthread_mutex_unlock(&stack->mutex);
        return false;
    }
    
    node->data = data;
    node->next = stack->top;
    stack->top = node;
    stack->size++;
    
    pthread_mutex_unlock(&stack->mutex);
    return true;
}

static void* stack_pop(ParseStack* stack) {
    if (!stack) return NULL;
    
    pthread_mutex_lock(&stack->mutex);
    
    if (!stack->top) {
        pthread_mutex_unlock(&stack->mutex);
        return NULL;
    }
    
    StackNode* node = stack->top;
    void* data = node->data;
    stack->top = node->next;
    stack->size--;
    free(node);
    
    pthread_mutex_unlock(&stack->mutex);
    return data;
}

/* =================================================================
 * DUAL-MODE PARSING ENGINE
 * =================================================================
 */

/* Parse tree node for recursive descent */
typedef struct ParseNode {
    uint32_t type;
    void* value;
    struct ParseNode* children;
    size_t child_count;
} ParseNode;

/* Top-down recursive descent parser */
static ParseNode* parse_top_down(DualModeParser* parser,
                                const char* input,
                                size_t* pos,
                                size_t length) {
    if (!parser || !input || *pos >= length) return NULL;
    
    /* Check recursion depth */
    if (parser->td_state.recursion_depth >= parser->td_state.max_recursion) {
        return NULL;
    }
    
    parser->td_state.recursion_depth++;
    atomic_fetch_add(&parser->stats.top_down_ops, 1);
    
    ParseNode* node = calloc(1, sizeof(ParseNode));
    if (!node) {
        parser->td_state.recursion_depth--;
        return NULL;
    }
    
    /* Try to match patterns in order */
    for (size_t i = 0; i < parser->pattern_count; i++) {
        RiftRegexPattern* pattern = parser->patterns[i];
        
        /* Only use patterns marked for top-down or dual mode */
        if (!(pattern->parse_mode & PARSE_MODE_TOP_DOWN)) continue;
        
        regmatch_t match;
        if (regexec(pattern->compiled_regex, input + *pos, 1, &match, 0) == 0) {
            if (match.rm_so == 0) {  /* Match at current position */
                node->type = i;  /* Pattern index as type */
                node->value = strndup(input + *pos, match.rm_eo - match.rm_so);
                *pos += match.rm_eo;
                
                /* Push to parse stack for tracking */
                if (parser->td_state.parse_stack) {
                    stack_push((ParseStack*)parser->td_state.parse_stack, node);
                }
                
                parser->td_state.recursion_depth--;
                return node;
            }
        }
    }
    
    /* No match found */
    free(node);
    parser->td_state.recursion_depth--;
    return NULL;
}

/* Bottom-up shift-reduce parser */
static bool parse_bottom_up(DualModeParser* parser,
                           const char* input,
                           size_t length,
                           TokenMemory* output,
                           size_t* output_count) {
    if (!parser || !input || !output) return false;
    
    ParseStack* reduce_stack = (ParseStack*)parser->bu_state.reduce_stack;
    if (!reduce_stack) return false;
    
    size_t pos = 0;
    size_t token_idx = 0;
    
    while (pos < length && token_idx < parser->bu_state.memory_size) {
        bool matched = false;
        
        /* Try each pattern */
        for (size_t i = 0; i < parser->pattern_count; i++) {
            RiftRegexPattern* pattern = parser->patterns[i];
            
            /* Only use patterns marked for bottom-up or dual mode */
            if (!(pattern->parse_mode & PARSE_MODE_BOTTOM_UP)) continue;
            
            regmatch_t match;
            if (regexec(pattern->compiled_regex, input + pos, 1, &match, 0) == 0) {
                if (match.rm_so == 0) {
                    /* Store token with memory */
                    TokenMemory* token = &output[token_idx];
                    token->token_type = i;
                    token->token_value = (uint32_t)match.rm_eo;
                    token->lexeme_start = pos;
                    token->lexeme_end = pos + match.rm_eo;
                    
                    /* Allocate and store lexeme in memory */
                    token->memory_value = strndup(input + pos, match.rm_eo);
                    
                    /* Push to reduce stack */
                    stack_push(reduce_stack, token);
                    
                    pos += match.rm_eo;
                    token_idx++;
                    matched = true;
                    
                    atomic_fetch_add(&parser->stats.bottom_up_ops, 1);
                    break;
                }
            }
        }
        
        if (!matched) {
            /* Skip unmatched character */
            pos++;
        }
    }
    
    *output_count = token_idx;
    return true;
}

/* =================================================================
 * MAIN PARSING FUNCTION WITH THREAD COORDINATION
 * =================================================================
 */

int rift_tb_parse_input(DualModeParser* parser,
                       const char* input,
                       size_t length,
                       TokenMemory** output_tokens,
                       size_t* token_count) {
    if (!parser || !input || !output_tokens || !token_count) return -1;
    
    pthread_mutex_lock(&parser->context_mutex);
    
    /* Allocate output buffer */
    *output_tokens = calloc(parser->bu_state.memory_size, sizeof(TokenMemory));
    if (!*output_tokens) {
        pthread_mutex_unlock(&parser->context_mutex);
        return -1;
    }
    
    /* Determine parsing strategy based on mode */
    if (parser->current_mode == PARSE_MODE_DUAL && parser->dual_mode_enabled) {
        /* Create thread contexts */
        ThreadContext contexts[THREAD_PAIR_COUNT];
        
        /* Thread 0: Top-down parser */
        contexts[0].eliminator = parser->parity_elim;
        contexts[0].mode = PARSE_MODE_TOP_DOWN;
        contexts[0].stack_trace = parser->td_state.parse_stack;
        contexts[0].heap_queue = pq_create(1024);
        contexts[0].thread_index = 0;
        
        /* Thread 1: Bottom-up parser */
        contexts[1].eliminator = parser->parity_elim;
        contexts[1].mode = PARSE_MODE_BOTTOM_UP;
        contexts[1].stack_trace = NULL;
        contexts[1].heap_queue = NULL;
        contexts[1].thread_index = 1;
        
        /* Launch threads with tbtbbt pattern */
        for (int i = 0; i < THREAD_PAIR_COUNT; i++) {
            pthread_create(&parser->parity_elim->threads[i], NULL,
                          thread_execute_pattern, &contexts[i]);
        }
        
        /* Parse with both modes concurrently */
        size_t td_pos = 0;
        ParseNode* td_result = parse_top_down(parser, input, &td_pos, length);
        
        size_t bu_count = 0;
        parse_bottom_up(parser, input, length, *output_tokens, &bu_count);
        
        /* Wait for thread completion */
        for (int i = 0; i < THREAD_PAIR_COUNT; i++) {
            pthread_join(parser->parity_elim->threads[i], NULL);
        }
        
        /* Merge results with YODA evaluation */
        for (size_t i = 0; i < bu_count; i++) {
            YodaEvalResult eval = rift_tb_yoda_evaluate(parser, 
                                                       &(*output_tokens)[i],
                                                       &parser->yoda_config);
            
            /* Apply evaluation result to token */
            if (eval == YODA_EVAL_INVARIANT) {
                (*output_tokens)[i].token_type |= 0x80000000;  /* Mark as invariant */
            }
        }
        
        *token_count = bu_count;
        
        /* Update parity elimination stats */
        atomic_fetch_add(&parser->stats.parity_eliminations, 1);
        
        /* Cleanup */
        if (td_result) {
            free(td_result->value);
            free(td_result);
        }
        
    } else if (parser->current_mode == PARSE_MODE_TOP_DOWN) {
        /* Top-down only */
        size_t pos = 0;
        size_t count = 0;
        
        while (pos < length && count < parser->bu_state.memory_size) {
            ParseNode* node = parse_top_down(parser, input, &pos, length);
            if (node) {
                (*output_tokens)[count].token_type = node->type;
                (*output_tokens)[count].token_value = (uint32_t)(uintptr_t)node->value;
                (*output_tokens)[count].lexeme_start = pos - strlen((char*)node->value);
                (*output_tokens)[count].lexeme_end = pos;
                (*output_tokens)[count].memory_value = node->value;
                count++;
                free(node);
            } else {
                pos++;  /* Skip unmatched */
            }
        }
        
        *token_count = count;
        
    } else if (parser->current_mode == PARSE_MODE_BOTTOM_UP) {
        /* Bottom-up only */
        parse_bottom_up(parser, input, length, *output_tokens, token_count);
    }
    
    pthread_mutex_unlock(&parser->context_mutex);
    return 0;
}

/* =================================================================
 * PARITY ELIMINATION IMPLEMENTATION
 * =================================================================
 */

void rift_tb_release_parity(ParityEliminator* elim, size_t thread_index) {
    if (!elim || thread_index >= THREAD_PAIR_COUNT) return;
    
    /* Flip parity bit for this thread */
    uint32_t thread_bit = 1 << thread_index;
    atomic_fetch_xor(&elim->parity_state, thread_bit);
    
    /* Release semaphore */
    sem_post(elim->parity_sem);
}

/* =================================================================
 * INVARIANT LOGIC SLICING
 * =================================================================
 */

bool rift_tb_invariant_slice(DualModeParser* parser,
                            size_t start_token,
                            size_t end_token,
                            void** slice_result) {
    if (!parser || !slice_result || start_token >= end_token) return false;
    
    pthread_mutex_lock(&parser->context_mutex);
    
    /* Calculate slice size */
    size_t slice_size = end_token - start_token;
    
    /* Allocate slice result */
    TokenMemory* slice = calloc(slice_size, sizeof(TokenMemory));
    if (!slice) {
        pthread_mutex_unlock(&parser->context_mutex);
        return false;
    }
    
    /* Apply invariant logic - slice problem in half */
    size_t mid_point = start_token + (slice_size / 2);
    
    /* Copy tokens with invariant analysis */
    for (size_t i = 0; i < slice_size; i++) {
        size_t src_idx = start_token + i;
        if (src_idx < parser->bu_state.memory_size) {
            slice[i] = parser->bu_state.token_memory[src_idx];
            
            /* Apply invariant marking based on position */
            if (src_idx < mid_point) {
                slice[i].token_type |= 0x40000000;  /* First half marker */
            } else {
                slice[i].token_type |= 0x20000000;  /* Second half marker */
            }
        }
    }
    
    *slice_result = slice;
    
    pthread_mutex_unlock(&parser->context_mutex);
    return true;
}

/* =================================================================
 * CLEANUP FUNCTIONS
 * =================================================================
 */

void rift_tb_parser_destroy(DualModeParser* parser) {
    if (!parser) return;
    
    pthread_mutex_lock(&parser->context_mutex);
    
    /* Clean up patterns */
    for (size_t i = 0; i < parser->pattern_count; i++) {
        if (parser->patterns[i]) {
            regfree(parser->patterns[i]->compiled_regex);
            free(parser->patterns[i]->compiled_regex);
            free(parser->patterns[i]->pattern_str);
            free(parser->patterns[i]);
        }
    }
    free(parser->patterns);
    
    /* Clean up parity eliminator */
    if (parser->parity_elim) {
        sem_close(parser->parity_elim->parity_sem);
        sem_unlink("/rift_parity");
        free(parser->parity_elim);
    }
    
    /* Clean up parse states */
    if (parser->td_state.parse_stack) {
        /* Pop all remaining nodes */
        ParseStack* stack = (ParseStack*)parser->td_state.parse_stack;
        while (stack->top) {
            stack_pop(stack);
        }
        pthread_mutex_destroy(&stack->mutex);
        free(stack);
    }
    
    if (parser->bu_state.reduce_stack) {
        ParseStack* stack = (ParseStack*)parser->bu_state.reduce_stack;
        while (stack->top) {
            stack_pop(stack);
        }
        pthread_mutex_destroy(&stack->mutex);
        free(stack);
    }
    
    /* Free token memory */
    if (parser->bu_state.token_memory) {
        for (size_t i = 0; i < parser->bu_state.memory_size; i++) {
            if (parser->bu_state.token_memory[i].memory_value) {
                free(parser->bu_state.token_memory[i].memory_value);
            }
        }
        free(parser->bu_state.token_memory);
    }
    
    pthread_mutex_unlock(&parser->context_mutex);
    pthread_mutex_destroy(&parser->context_mutex);
    
    free(parser);
}