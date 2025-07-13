
#ifndef RIFT_R_UML_H
#define RIFT_R_UML_H

/**
 * r_uml_patterns Module Header
 * RIFT1 - AEGIS Framework
 * OBINexus Computing
 *
 * Auto-generated header for QA compliance
 */

#include <stddef.h>
#include <stdbool.h>
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

#ifdef __cplusplus
extern "C" {
#endif

// RIFT-style regex pattern macros (C11-compliant)
#define R_COMPOSITION_PATTERN "([A-Za-z_][A-Za-z0-9_]*)\\s+composed_of\\s+([A-Za-z_][A-Za-z0-9_]*)"
#define R_ASSOCIATION_PATTERN "([A-Za-z_][A-Za-z0-9_]*)\\s+associated_with\\s+([A-Za-z_][A-Za-z0-9_]*)"
#define R_AGGREGATION_PATTERN "([A-Za-z_][A-Za-z0-9_]*)\\s+aggregate\\s+([A-Za-z_][A-Za-z0-9_]*)"
#define R_INHERITANCE_PATTERN "([A-Za-z_][A-Za-z0-9_]*)\\s+(extends|inherits)\\s+([A-Za-z_][A-Za-z0-9_]*)"

// Boolean and logical operation macros for analytics and governance
#define R_AND(a, b) ((a) && (b))
#define R_OR(a, b)  ((a) || (b))
#define R_NOT(a)    (!(a))

// Token type mapping (example)
typedef enum {
    R_INIT_EMPTY = 0,
    R_UML_COMPOSITION,
    R_UML_ASSOCIATION,
    R_UML_AGGREGATION,
    R_UML_INHERITANCE,
    R_END_EOFR
} rift_uml_token_type_t;

// Relationship type enum for internal use
typedef enum {
    UML_COMPOSITION = 1,
    UML_ASSOCIATION,
    UML_AGGREGATION,
    UML_INHERITANCE
} uml_relationship_type_t;

// UML relationship struct definition
typedef struct uml_relationship {
    char source_class[64];
    char target_class[64];
    uml_relationship_type_t relationship;
    bool lifecycle_dependency;
} uml_relationship_t;

// Governance triangle struct definition
typedef struct governance_triangle {
    float attack_risk;
    float rollback_cost;
    float stability_impact;
} governance_triangle_t;

// Function prototypes (public API)
uml_relationship_t *parse_uml_relationship(const char *pattern, const char *source);
bool validate_uml_governance(uml_relationship_t *rel);
void generate_uml_code(uml_relationship_t *rel, char *output_buffer, size_t buffer_size);

// Governance compliance checker
bool is_governance_compliant(const governance_triangle_t *triangle);

// Boolean helpers for CLI/core analytics
static inline bool rift_and(bool a, bool b) { return a && b; }
static inline bool rift_or(bool a, bool b) { return a || b; }
static inline bool rift_not(bool a) { return !a; }

#ifdef __cplusplus
}
#endif

#endif // RIFT_R_UML_H
