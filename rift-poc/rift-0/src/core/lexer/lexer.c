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

/*
 * RIFT-Core-0 Lexer Module
 * Handles Stage-0 lexical analysis (tokenization) using RIFT regular expressions (R""/r"").
 * Governance: #[gov:stage_0_core]
 */


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




// Helper functions
static size_t next_id = 1;
static size_t generate_id(void) {
    return next_id++;
}

// State functions
State* state_create(const char* pattern, bool is_final) {
    State* state = malloc(sizeof(State));
    if (!state) return NULL;
    
    state->pattern = strdup(pattern);
    if (!state->pattern) {
        free(state);
        return NULL;
    }
    
    state->is_final = is_final;
    state->id = generate_id();
    return state;
}

void state_destroy(State* state) {
    if (!state) return;
    free(state->pattern);
    free(state);
}

bool state_matches(State* state, const char* text) {
    if (!state || !text) return false;
    
    regex_t regex;
    int result;
    
    result = regcomp(&regex, state->pattern, REG_EXTENDED);
    if (result) return false;
    
    result = regexec(&regex, text, 0, NULL, 0);
    regfree(&regex);
    
    return (result == 0);
}

// Automaton functions
RegexAutomaton* automaton_create(void) {
    RegexAutomaton* automaton = malloc(sizeof(RegexAutomaton));
    if (!automaton) return NULL;
    
    automaton->state_capacity = 10;
    automaton->states = malloc(sizeof(State*) * automaton->state_capacity);
    if (!automaton->states) {
        free(automaton);
        return NULL;
    }
    
    automaton->transition_capacity = 20;
    automaton->transitions = malloc(sizeof(Transition*) * automaton->transition_capacity);
    if (!automaton->transitions) {
        free(automaton->states);
        free(automaton);
        return NULL;
    }
    
    automaton->state_count = 0;
    automaton->transition_count = 0;
    automaton->initial_state = NULL;
    automaton->current_state = NULL;
    
    return automaton;
}

void automaton_destroy(RegexAutomaton* automaton) {
    if (!automaton) return;
    
    for (size_t i = 0; i < automaton->state_count; i++) {
        state_destroy(automaton->states[i]);
    }
    free(automaton->states);
    
    for (size_t i = 0; i < automaton->transition_count; i++) {
        free(automaton->transitions[i]->input_pattern);
        free(automaton->transitions[i]);
    }
    free(automaton->transitions);
    
    free(automaton);
}

State* automaton_add_state(RegexAutomaton* automaton, const char* pattern, bool is_final) {
    if (!automaton || !pattern) return NULL;
    
    // Check capacity
    if (automaton->state_count >= automaton->state_capacity) {
        size_t new_capacity = automaton->state_capacity * 2;
        State** new_states = realloc(automaton->states, sizeof(State*) * new_capacity);
        if (!new_states) return NULL;
        
        automaton->states = new_states;
        automaton->state_capacity = new_capacity;
    }
    
    State* state = state_create(pattern, is_final);
    if (!state) return NULL;
    
    automaton->states[automaton->state_count++] = state;
    
    if (!automaton->initial_state) {
        automaton->initial_state = state;
        automaton->current_state = state;
    }
    
    return state;
}

bool automaton_add_transition(RegexAutomaton* automaton, State* from, const char* pattern, State* to) {
    if (!automaton || !from || !pattern || !to) return false;
    
    // Check capacity
    if (automaton->transition_count >= automaton->transition_capacity) {
        size_t new_capacity = automaton->transition_capacity * 2;
        Transition** new_transitions = realloc(automaton->transitions, 
                                             sizeof(Transition*) * new_capacity);
        if (!new_transitions) return false;
        
        automaton->transitions = new_transitions;
        automaton->transition_capacity = new_capacity;
    }
    
    Transition* transition = malloc(sizeof(Transition));
    if (!transition) return false;
    
    transition->from_state = from;
    transition->input_pattern = strdup(pattern);
    if (!transition->input_pattern) {
        free(transition);
        return false;
    }
    transition->to_state = to;
    
    automaton->transitions[automaton->transition_count++] = transition;
    return true;
}

State* automaton_get_next_state(RegexAutomaton* automaton, const char* input) {
    if (!automaton || !automaton->current_state || !input) return NULL;
    
    // Try direct pattern matching first
    for (size_t i = 0; i < automaton->state_count; i++) {
        if (state_matches(automaton->states[i], input)) {
            automaton->current_state = automaton->states[i];
            return automaton->states[i];
        }
    }
    
    return NULL;
}

// IR Generator functions
IRGenerator* ir_generator_create(RegexAutomaton* automaton) {
    if (!automaton) return NULL;
    
    IRGenerator* generator = malloc(sizeof(IRGenerator));
    if (!generator) return NULL;
    
    generator->automaton = automaton;
    generator->node_capacity = 10;
    generator->nodes = malloc(sizeof(TokenNode*) * generator->node_capacity);
    if (!generator->nodes) {
        free(generator);
        return NULL;
    }
    
    generator->node_count = 0;
    return generator;
}

void ir_generator_destroy(IRGenerator* generator) {
    if (!generator) return;
    
    for (size_t i = 0; i < generator->node_count; i++) {
        free(generator->nodes[i]->type);
        free(generator->nodes[i]->value);
        free(generator->nodes[i]);
    }
    
    free(generator->nodes);
    free(generator);
}

TokenNode* ir_generator_process_token(IRGenerator* generator, const char* token) {
    if (!generator || !token) return NULL;
    
    State* next_state = automaton_get_next_state(generator->automaton, token);
    if (!next_state) return NULL;
    
    TokenNode* node = malloc(sizeof(TokenNode));
    if (!node) return NULL;
    
    node->type = strdup(next_state->pattern);
    node->value = strdup(token);
    
    if (!node->type || !node->value) {
        free(node->type);
        free(node->value);
        free(node);
        return NULL;
    }
    
    return node;
}

