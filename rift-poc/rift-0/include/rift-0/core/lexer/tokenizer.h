#ifndef RIFT_TOKENIZER_H
#define RIFT_TOKENIZER_H

/* Include types first */
#include "rift-0/core/lexer/tokenizer_types.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Tokenizer API functions */
TokenizerContext* rift_tokenizer_create(void);
TokenizerContext* rift_tokenizer_create_with_capacity(size_t token_capacity,
                                                      size_t pattern_capacity);
void rift_tokenizer_destroy(TokenizerContext* ctx);

/* Tokenization functions */
ssize_t rift_tokenizer_process_with_flags(TokenizerContext* ctx,
                                         const char* input,
                                         size_t length,
                                         TokenFlags flags);

/* Token access */
size_t rift_tokenizer_get_tokens(const TokenizerContext* ctx,
                                TokenTriplet* tokens,
                                size_t max_tokens);

bool rift_tokenizer_get_token_at(const TokenizerContext* ctx,
                                size_t index,
                                TokenTriplet* token);

/* Configuration */
bool rift_tokenizer_set_flags(TokenizerContext* ctx, TokenFlags flags);
TokenFlags rift_tokenizer_get_flags(const TokenizerContext* ctx);
bool rift_tokenizer_set_debug_mode(TokenizerContext* ctx, bool enable);
bool rift_tokenizer_set_strict_mode(TokenizerContext* ctx, bool strict);
bool rift_tokenizer_set_thread_safe_mode(TokenizerContext* ctx, bool thread_safe);

/* Error handling */
bool rift_tokenizer_has_error(const TokenizerContext* ctx);
const char* rift_tokenizer_get_error_message(const TokenizerContext* ctx);
TokenizerErrorCode rift_tokenizer_get_error_code(const TokenizerContext* ctx);
void rift_tokenizer_clear_error(TokenizerContext* ctx);

/* Utility functions */
const char* rift_tokenizer_get_version(void);
const char* rift_tokenizer_token_type_to_string(TokenType token_type);
bool rift_tokenizer_validate_context(const TokenizerContext* ctx);
bool rift_tokenizer_reset(TokenizerContext* ctx);

/* Thread safety */
bool rift_tokenizer_lock(TokenizerContext* ctx);
bool rift_tokenizer_unlock(TokenizerContext* ctx);
bool rift_tokenizer_trylock(TokenizerContext* ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_TOKENIZER_H */
