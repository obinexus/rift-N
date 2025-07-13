
/* RIFT_CLEANUP: Stub header for rift_tokenizer.h */
#ifndef RIFT_TOKENIZER_H_STUB
#define RIFT_TOKENIZER_H_STUB

#include <stddef.h>
#include <stdint.h>

/* AEGIS framework types */
typedef struct rift_tokenizer_context_t rift_tokenizer_context_t;
typedef struct rift_tokenizer_config_t rift_tokenizer_config_t;
typedef int rift_tokenizer_result_t;

/* Result codes */
#define RIFT_TOKENIZER_SUCCESS 0
#define RIFT_TOKENIZER_ERROR_INVALID_INPUT -1
#define RIFT_TOKENIZER_ERROR_MEMORY -2
#define RIFT_TOKENIZER_ERROR_VALIDATION -3

/* Version info */
#define RIFT_TOKENIZER_VERSION 0x00010000
#define DEFAULT_THREAD_COUNT 4

#endif /* RIFT_TOKENIZER_H_STUB */
