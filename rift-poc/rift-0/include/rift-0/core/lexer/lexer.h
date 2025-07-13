
#include <stdbool.h>
#include <stdint.h>
#include <regex.h>
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
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif



// Structure definitions
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


// Token types for type-safe tokenization
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
    void* memory; // For memory-governed allocation
} TokenNode;

// Lexer flags for modular feature control
typedef enum {
    LEXER_FLAG_NONE = 0,
    LEXER_FLAG_RAW_STRING = 1 << 0,
    LEXER_FLAG_RAW_CHAR = 1 << 1,
    LEXER_FLAG_GLOBAL_TB = 1 << 2, // [tb] global match
    LEXER_FLAG_TOP_DOWN = 1 << 3,
    LEXER_FLAG_SHIFT_REDUCE = 1 << 4
} LexerFlags;

// Lexer context for modular parsing
typedef struct {
    RegexAutomaton* automaton;
    LexerFlags flags;
    size_t token_count;
    TokenNode** tokens;
} LexerContext;

// API for flag management
void lexer_set_flag(LexerContext* ctx, LexerFlags flag);
void lexer_clear_flag(LexerContext* ctx, LexerFlags flag);
bool lexer_flag_enabled(const LexerContext* ctx, LexerFlags flag);

// Modular token creation
TokenNode* token_create(TokenType type, const char* value, size_t length);
void token_destroy(TokenNode* token);

typedef struct IRGenerator {
    RegexAutomaton* automaton;
    TokenNode** nodes;
    size_t node_count;
    size_t node_capacity;
} IRGenerator;


// ...existing code...

#ifdef __cplusplus
}
#endif