#ifndef RIFT_TOKENIZER_RULES_H
#define RIFT_TOKENIZER_RULES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "rift-0/core/lexer/tokenizer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DFA State Machine API */
DFAStateMachine* dfa_create(void);
void dfa_destroy(DFAStateMachine* dfa);
bool dfa_process_char(DFAStateMachine* dfa, char c);
bool dfa_is_accepting(DFAStateMachine* dfa);
void dfa_reset(DFAStateMachine* dfa);

/* Rule system initialization */
int init_tokenizer_rules(void);
void cleanup_tokenizer_rules(void);

/* Token pattern matching */
int match_token_pattern(const char* src, TokenTriplet* out_token);
int match_token_pattern_ex(const char* src, const char* pattern, 
                          uint32_t flags, PatternMatchResult* result);

/* Tokenization functions */
TokenizationResult tokenize_source(const char* src, size_t length);
int tokenize_source_into(const char* src, TokenTriplet* tokens, 
                        size_t max_tokens, size_t* token_count);
void free_tokenization_result(TokenizationResult* result);

/* NULL/nil semantic validation */
TokenType classify_null_nil_semantic(const char* text, size_t len);
bool validate_null_nil_semantics(const char* text, TokenType* semantic_type);

/* Token validation and encoding */
TokenTriplet R_encode_safe(TokenType type, uint16_t mem_ptr, uint8_t value);
bool validate_token_separation(const TokenTriplet* token);

/* Extended tokenizer API for tests */
int tokenizer_initialize(void);
void tokenizer_cleanup(void);
int tokenizer_process_string(const char* input, TokenTriplet* tokens, 
                           size_t max, size_t* count);
bool tokenizer_has_error(void);
const char* tokenizer_get_error_message(void);
void tokenizer_reset_error(void);

/* Policy validation */
bool polic_validate_token(const TokenTriplet* token, void* ctx);

/* Boolean composition */
int evaluate_boolean_composition(const char* expr, bool* result);

/* DFA State operations */
DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final);
void rift_dfa_destroy_states(DFAState* root);
bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char);
DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length);
bool rift_dfa_is_accepting_state(DFAState* state);
TokenType rift_dfa_get_token_type(DFAState* state);
bool rift_dfa_set_token_type(DFAState* state, TokenType token_type);

/* Regex compilation */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags);
void rift_regex_destroy(RegexComposition* regex);
bool rift_regex_match(const RegexComposition* regex, const char* input, size_t length);
size_t rift_regex_extract_matches(const RegexComposition* regex,
                                 const char* input, size_t length,
                                 TokenTriplet* matches, size_t max_matches);

/* Token operations */
TokenTriplet rift_token_create(uint8_t type, uint16_t mem_ptr, uint8_t value);
bool rift_token_validate(const TokenTriplet* token);
TokenType rift_token_get_type(const TokenTriplet* token);
uint16_t rift_token_get_mem_ptr(const TokenTriplet* token);
uint8_t rift_token_get_value(const TokenTriplet* token);
TokenTriplet rift_token_set_flags(TokenTriplet token, TokenFlags flags);

/* Rules management */
bool rift_rules_register_pattern(TokenizerContext* ctx,
                               const char* pattern,
                               TokenType token_type,
                               TokenFlags flags);
bool rift_rules_unregister_pattern(TokenizerContext* ctx, const char* pattern);
ssize_t rift_rules_apply_all(TokenizerContext* ctx, const char* input, size_t length);
size_t rift_rules_get_count(const TokenizerContext* ctx);
bool rift_rules_clear_all(TokenizerContext* ctx);
const char* rift_rules_get_last_error(const TokenizerContext* ctx);
TokenizerErrorCode rift_rules_get_last_error_code(const TokenizerContext* ctx);
void rift_rules_clear_error(TokenizerContext* ctx);

/* Policy2 QA validation */
int policy2_qa_validate(const char* test_input, TokenType expected_type, 
                       const char* test_category);
int run_policy2_qa_matrix(bool verbose);
int export_qa_results_csv(const char* filename);

/* Constants for testing */
#define RIFT_TOKENIZER_VERSION "0.1.0"
#define DFA_STATE_INIT 0
#define DFA_STATE_ERROR -1

#ifdef __cplusplus
}
#endif

#endif /* RIFT_TOKENIZER_RULES_H */
