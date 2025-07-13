#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <pthread.h>

#include "rift-0/core/lexer/tokenizer_rules.h"
#include "rift-0/core/lexer/tokenizer.h"

/* Global state for tokenizer rules */
static struct {
    bool initialized;
    pthread_mutex_t rules_mutex;
    char error_message[512];
    TokenizerErrorCode error_code;
    bool has_error;
} g_tokenizer_state = {
    .initialized = false,
    .rules_mutex = PTHREAD_MUTEX_INITIALIZER,
    .error_code = RIFT_TOKENIZER_ERROR_NONE,
    .has_error = false
};

/**
 * Initialize tokenizer rules system
 */
int init_tokenizer_rules(void) {
    pthread_mutex_lock(&g_tokenizer_state.rules_mutex);
    
    if (g_tokenizer_state.initialized) {
        pthread_mutex_unlock(&g_tokenizer_state.rules_mutex);
        return 0;
    }
    
    /* Initialize rule patterns */
    g_tokenizer_state.initialized = true;
    g_tokenizer_state.has_error = false;
    g_tokenizer_state.error_code = RIFT_TOKENIZER_ERROR_NONE;
    g_tokenizer_state.error_message[0] = '\0';
    
    pthread_mutex_unlock(&g_tokenizer_state.rules_mutex);
    return 0;
}

/**
 * Cleanup tokenizer rules
 */
void cleanup_tokenizer_rules(void) {
    pthread_mutex_lock(&g_tokenizer_state.rules_mutex);
    g_tokenizer_state.initialized = false;
    pthread_mutex_unlock(&g_tokenizer_state.rules_mutex);
}

/**
 * DFA State Machine implementation
 */
struct DFAStateMachine {
    int state;
    DFAState* start_state;
    DFAState* current_state;
};

DFAStateMachine* dfa_create(void) {
    DFAStateMachine* dfa = malloc(sizeof(DFAStateMachine));
    if (dfa) {
        dfa->state = DFA_STATE_INIT;
        dfa->start_state = NULL;
        dfa->current_state = NULL;
    }
    return dfa;
}

void dfa_destroy(DFAStateMachine* dfa) {
    if (dfa) {
        if (dfa->start_state) {
            rift_dfa_destroy_states(dfa->start_state);
        }
        free(dfa);
    }
}

bool dfa_process_char(DFAStateMachine* dfa, char c) {
    if (!dfa) return false;
    
    /* Simple R pattern detection */
    switch (dfa->state) {
        case DFA_STATE_INIT:
            if (c == 'R') {
                dfa->state = 1;
                return true;
            }
            break;
        case 1:
            if (c == '"' || c == '\'') {
                dfa->state = 2;
                return true;
            }
            break;
        case 2:
            /* Accept any character in pattern */
            return true;
    }
    
    dfa->state = DFA_STATE_ERROR;
    return false;
}

bool dfa_is_accepting(DFAStateMachine* dfa) {
    return dfa && dfa->state >= 2;
}

void dfa_reset(DFAStateMachine* dfa) {
    if (dfa) {
        dfa->state = DFA_STATE_INIT;
        dfa->current_state = dfa->start_state;
    }
}

/**
 * Token pattern matching
 */
int match_token_pattern(const char* src, TokenTriplet* out_token) {
    if (!src || !out_token) return -1;
    
    /* Initialize output */
    out_token->type = TOKEN_UNKNOWN;
    out_token->mem_ptr = 0;
    out_token->value = 0;
    
    /* Skip whitespace */
    while (*src && isspace(*src)) src++;
    
    if (!*src) return 0;
    
    /* Check for R pattern */
    if (src[0] == 'R' && (src[1] == '"' || src[1] == '\'')) {
        out_token->type = TOKEN_R_PATTERN;
        out_token->mem_ptr = 0;
        out_token->value = 0;
        
        /* Find pattern end */
        char delim = src[1];
        const char* end = strchr(src + 2, delim);
        if (end) {
            return (end - src) + 1;
        }
        return 2; /* Consume R" at least */
    }
    
    /* Identifier */
    if (isalpha(*src) || *src == '_') {
        out_token->type = TOKEN_IDENTIFIER;
        int len = 1;
        while (src[len] && (isalnum(src[len]) || src[len] == '_')) {
            len++;
        }
        
        /* Check for NULL/nil keywords */
        if (len == 4 && strncasecmp(src, "NULL", 4) == 0) {
            out_token->type = TOKEN_NULL_KEYWORD;
        } else if (len == 3 && strncasecmp(src, "nil", 3) == 0) {
            out_token->type = TOKEN_NIL_KEYWORD;
        }
        
        return len;
    }
    
    /* Number */
    if (isdigit(*src)) {
        out_token->type = TOKEN_LITERAL_NUMBER;
        int len = 1;
        while (src[len] && (isdigit(src[len]) || src[len] == '.')) {
            len++;
        }
        return len;
    }
    
    /* String literal */
    if (*src == '"') {
        out_token->type = TOKEN_LITERAL_STRING;
        int len = 1;
        while (src[len] && src[len] != '"') {
            if (src[len] == '\\' && src[len + 1]) {
                len += 2;
            } else {
                len++;
            }
        }
        if (src[len] == '"') len++;
        return len;
    }
    
    /* Operators and delimiters */
    const char* operators = "+-*/=<>!&|";
    const char* delimiters = "(){}[];,";
    
    if (strchr(operators, *src)) {
        out_token->type = TOKEN_OPERATOR;
        return 1;
    }
    
    if (strchr(delimiters, *src)) {
        out_token->type = TOKEN_DELIMITER;
        return 1;
    }
    
    /* Unknown character */
    out_token->type = TOKEN_UNKNOWN;
    return 1;
}

/**
 * Tokenize complete source
 */
TokenizationResult tokenize_source(const char* src, size_t length) {
    TokenizationResult result = {0};
    
    if (!src || length == 0) {
        result.success = false;
        result.error_message = strdup("Empty input");
        return result;
    }
    
    /* Allocate token array */
    result.tokens = malloc(length * sizeof(TokenTriplet));
    if (!result.tokens) {
        result.success = false;
        result.error_message = strdup("Memory allocation failed");
        return result;
    }
    
    /* Tokenize */
    size_t pos = 0;
    result.count = 0;
    
    while (pos < length) {
        TokenTriplet token;
        int consumed = match_token_pattern(src + pos, &token);
        
        if (consumed > 0) {
            token.mem_ptr = (uint16_t)pos;
            result.tokens[result.count++] = token;
            pos += consumed;
        } else if (consumed == 0) {
            /* Skip whitespace */
            pos++;
        } else {
            /* Error */
            result.success = false;
            result.error_message = strdup("Tokenization error");
            free(result.tokens);
            result.tokens = NULL;
            result.count = 0;
            return result;
        }
    }
    
    result.success = true;
    return result;
}

/**
 * Free tokenization result
 */
void free_tokenization_result(TokenizationResult* result) {
    if (result) {
        if (result->tokens) {
            free(result->tokens);
            result->tokens = NULL;
        }
        if (result->error_message) {
            free(result->error_message);
            result->error_message = NULL;
        }
        result->count = 0;
        result->success = false;
    }
}

/**
 * NULL/nil semantic classification
 */
TokenType classify_null_nil_semantic(const char* text, size_t len) {
    if (!text) return TOKEN_UNKNOWN;
    
    if (len == 4 && strncasecmp(text, "NULL", 4) == 0) {
        return TOKEN_NULL_KEYWORD;
    } else if (len == 3 && strncasecmp(text, "nil", 3) == 0) {
        return TOKEN_NIL_KEYWORD;
    }
    
    return TOKEN_UNKNOWN;
}

/**
 * Token encoding and validation
 */
TokenTriplet R_encode_safe(TokenType type, uint16_t mem_ptr, uint8_t value) {
    TokenTriplet token;
    token.type = (uint8_t)type;
    token.mem_ptr = mem_ptr;
    token.value = value;
    return token;
}

bool validate_token_separation(const TokenTriplet* token) {
    if (!token) return false;
    if (token->type >= TOKEN_ERROR) return false;
    return true;
}

/**
 * Extended tokenizer API implementations
 */
static TokenizerContext* g_test_tokenizer = NULL;

int tokenizer_initialize(void) {
    if (g_test_tokenizer) {
        rift_tokenizer_destroy(g_test_tokenizer);
    }
    g_test_tokenizer = rift_tokenizer_create();
    return g_test_tokenizer ? 0 : -1;
}

void tokenizer_cleanup(void) {
    if (g_test_tokenizer) {
        rift_tokenizer_destroy(g_test_tokenizer);
        g_test_tokenizer = NULL;
    }
}

int tokenizer_process_string(const char* input, TokenTriplet* tokens, 
                           size_t max, size_t* count) {
    if (!input || !tokens || !count || !g_test_tokenizer) {
        return -1;
    }
    
    *count = 0;
    size_t pos = 0;
    size_t len = strlen(input);
    
    while (pos < len && *count < max) {
        int consumed = match_token_pattern(input + pos, &tokens[*count]);
        if (consumed > 0) {
            tokens[*count].mem_ptr = (uint16_t)pos;
            (*count)++;
            pos += consumed;
        } else if (consumed == 0) {
            pos++; /* Skip whitespace */
        } else {
            return -1; /* Error */
        }
        
        /* Skip whitespace */
        while (pos < len && isspace(input[pos])) {
            pos++;
        }
    }
    
    /* Add EOF token */
    if (*count < max) {
        tokens[*count].type = TOKEN_EOF;
        tokens[*count].mem_ptr = (uint16_t)len;
        tokens[*count].value = 0;
        (*count)++;
    }
    
    return 0;
}

bool tokenizer_has_error(void) {
    return g_tokenizer_state.has_error;
}

const char* tokenizer_get_error_message(void) {
    return g_tokenizer_state.error_message;
}

void tokenizer_reset_error(void) {
    pthread_mutex_lock(&g_tokenizer_state.rules_mutex);
    g_tokenizer_state.has_error = false;
    g_tokenizer_state.error_code = RIFT_TOKENIZER_ERROR_NONE;
    g_tokenizer_state.error_message[0] = '\0';
    pthread_mutex_unlock(&g_tokenizer_state.rules_mutex);
}

/**
 * Policy validation stub
 */
bool polic_validate_token(const TokenTriplet* token, void* ctx) {
    if (!token) return false;
    return token->type < TOKEN_ERROR;
}

/**
 * Boolean composition evaluation stub
 */
int evaluate_boolean_composition(const char* expr, bool* result) {
    if (!expr || !result) return -1;
    *result = false;
    /* TODO: Implement boolean expression evaluation */
    return 0;
}

/**
 * Extended pattern matching
 */
int match_token_pattern_ex(const char* src, const char* pattern, 
                          uint32_t flags, PatternMatchResult* result) {
    if (!src || !pattern || !result) return -1;
    
    result->matched = false;
    result->match_length = 0;
    result->token_type = TOKEN_UNKNOWN;
    
    /* TODO: Implement pattern-based matching */
    return 0;
}

/**
 * NULL/nil validation
 */
bool validate_null_nil_semantics(const char* text, TokenType* semantic_type) {
    if (!text || !semantic_type) return false;
    
    *semantic_type = classify_null_nil_semantic(text, strlen(text));
    return (*semantic_type == TOKEN_NULL_KEYWORD || 
            *semantic_type == TOKEN_NIL_KEYWORD);
}

/**
 * DFA State management
 */
DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final) {
    DFAState* state = malloc(sizeof(DFAState));
    if (state) {
        state->state_id = state_id;
        state->is_final = is_final;
        state->is_start = false;
        state->transition_char = '\0';
        state->next_state = NULL;
        state->fail_state = NULL;
        state->token_type = TOKEN_UNKNOWN;
        state->match_count = 0;
    }
    return state;
}

void rift_dfa_destroy_states(DFAState* root) {
    /* TODO: Implement proper tree destruction */
    if (root) {
        if (root->next_state) {
            rift_dfa_destroy_states(root->next_state);
        }
        free(root);
    }
}

bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char) {
    if (!from || !to) return false;
    /* TODO: Implement transition table */
    return true;
}

DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length) {
    if (!start || !input) return NULL;
    /* TODO: Implement DFA processing */
    return start;
}

bool rift_dfa_is_accepting_state(DFAState* state) {
    return state && state->is_final;
}

TokenType rift_dfa_get_token_type(DFAState* state) {
    return state ? state->token_type : TOKEN_UNKNOWN;
}

bool rift_dfa_set_token_type(DFAState* state, TokenType token_type) {
    if (!state) return false;
    state->token_type = token_type;
    return true;
}

/**
 * Regex compilation stubs
 */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags) {
    if (!pattern) return NULL;
    
    RegexComposition* regex = malloc(sizeof(RegexComposition));
    if (regex) {
        regex->is_compiled = true;
        regex->start_state = rift_dfa_create_state(0, false);
        regex->pattern = strdup(pattern);
        regex->flags = flags;
    }
    return regex;
}

void rift_regex_destroy(RegexComposition* regex) {
    if (regex) {
        if (regex->start_state) {
            rift_dfa_destroy_states(regex->start_state);
        }
        if (regex->pattern) {
            free(regex->pattern);
        }
        free(regex);
    }
}

bool rift_regex_match(const RegexComposition* regex, const char* input, size_t length) {
    if (!regex || !regex->is_compiled || !input) return false;
    /* TODO: Implement regex matching */
    return false;
}

/**
 * Additional utility functions for testing
 */
int tokenize_source_into(const char* src, TokenTriplet* tokens, 
                        size_t max_tokens, size_t* token_count) {
    if (!src || !tokens || !token_count) return -1;
    
    TokenizationResult result = tokenize_source(src, strlen(src));
    if (!result.success) {
        free_tokenization_result(&result);
        return -1;
    }
    
    *token_count = result.count < max_tokens ? result.count : max_tokens;
    memcpy(tokens, result.tokens, *token_count * sizeof(TokenTriplet));
    
    free_tokenization_result(&result);
    return 0;
}

/* Policy2 QA validation stubs */
int policy2_qa_validate(const char* test_input, TokenType expected_type, 
                       const char* test_category) {
    /* TODO: Implement Policy2 validation */
    return 0;
}

int run_policy2_qa_matrix(bool verbose) {
    /* TODO: Implement QA matrix */
    return 0;
}

int export_qa_results_csv(const char* filename) {
    /* TODO: Implement CSV export */
    return 0;
}

/* Token operations */
TokenTriplet rift_token_create(uint8_t type, uint16_t mem_ptr, uint8_t value) {
    return R_encode_safe((TokenType)type, mem_ptr, value);
}

bool rift_token_validate(const TokenTriplet* token) {
    return validate_token_separation(token);
}

TokenType rift_token_get_type(const TokenTriplet* token) {
    return token ? (TokenType)token->type : TOKEN_UNKNOWN;
}

uint16_t rift_token_get_mem_ptr(const TokenTriplet* token) {
    return token ? token->mem_ptr : 0;
}

uint8_t rift_token_get_value(const TokenTriplet* token) {
    return token ? token->value : 0;
}
