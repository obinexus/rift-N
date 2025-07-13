/* Auto-generated token pattern definitions */
#ifndef RIFT_TOKEN_PATTERNS_H
#define RIFT_TOKEN_PATTERNS_H

#include "rift-0/core/lexer/tokenizer_types.h"

typedef struct {
    const char* name;
    const char* pattern;
    RiftTokenType type;
    int is_quantum;
    int priority;
} TokenPatternDef;

static const TokenPatternDef stage0_patterns[] = {
    /* Classical patterns */
    {"identifier", "^[a-zA-Z_][a-zA-Z0-9_]*$", R_IDENTIFIER, 0, 100},
    {"number", "^[0-9]+(\\.[0-9]+)?$", R_NUMBER, 0, 90},
    {"operator", "^[+\\-*/=<>!&|^~%]$", R_OPERATOR, 0, 80},
    {"string", "^\"[^\"]*\"$", R_STRING, 0, 85},
    {"comment", "^//.*$", R_COMMENT, 0, 70},
    {"whitespace", "^[ \\t\\n\\r]+$", R_WHITESPACE, 0, 60},
    
    /* Quantum patterns */
    {"quantum_init", "^@quantum\\s*\\{$", R_QUANTUM_TOKEN, 1, 150},
    {"collapse", "^!collapse$", R_COLLAPSE_MARKER, 1, 140},
    {"entangle", "^@entangle$", R_ENTANGLE_MARKER, 1, 130},
    
    /* Governance patterns */
    {"governance", "^#\\[gov:[^\\]]+\\]$", R_GOVERNANCE_TAG, 0, 120},
    
    /* Terminator */
    {NULL, NULL, R_INIT_EMPTY, 0, 0}
};

extern const size_t stage0_patterns_count;

#endif /* RIFT_TOKEN_PATTERNS_H */
