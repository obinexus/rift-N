#ifndef LEXER_FLAG_H
#define LEXER_FLAG_H

#ifdef __cplusplus
extern "C" {
#endif


// =====================
// Lexer Flag API (modular, thread-safe)
// =====================
#include "rift-0/core/lexer/lexer.h"

void lexer_set_flag(LexerContext* ctx, int flag); /* RIFT_CLEANUP: Fixed incomplete declaration */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

// =====================
// Heap-based Priority Queue (thread-safe)
// =====================

typedef struct Item {
    int priority;
    int value;
} Item;

typedef struct HeapQueue {
    Item* heap;
    int size;
    int capacity;
    pthread_mutex_t lock;
} HeapQueue;

typedef struct State {
    char* pattern;
    bool is_final;
    size_t id;
} State;

typedef struct Transition {
    State* from_state;
    char* input_pattern;
    State* to_state;
} Transition;

typedef struct RegexAutomaton {
    State** states;
    size_t state_count;
    size_t state_capacity;
    Transition** transitions;
    size_t transition_count;
    size_t transition_capacity;
    State* initial_state;
    State* current_state;
} RegexAutomaton;

typedef enum {
    TOKEN_TYPE_UNKNOWN = 0,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_CHAR,
    TOKEN_TYPE_RAW_STRING,
    TOKEN_TYPE_RAW_CHAR,
    TOKEN_TYPE_WHITESPACE,
    TOKEN_TYPE_SPECIAL,
    TOKEN_TYPE_EOF
} TokenType;

typedef struct TokenNode {
    TokenType type;
    char* value;
    size_t length;
    void* memory;
} TokenNode;

HeapQueue* hq_create(int capacity);
void hq_destroy(HeapQueue* hq);
void hq_enqueue(HeapQueue* hq, int priority, int value);
Item hq_dequeue(HeapQueue* hq);

Flags flag);
void lexer_clear_flag(LexerContext* ctx, LexerFlags flag);
bool lexer_flag_enabled(const LexerContext* ctx, LexerFlags flag);

TokenNode* token_create(TokenType type, const char* value, size_t length);
void token_destroy(TokenNode* token);

#ifdef __cplusplus
}
#endif // End of extern "C"

#endif // LEXER_FLAG_H
