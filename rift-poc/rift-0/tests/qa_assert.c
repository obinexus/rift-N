/*
 * RIFT Stage-0 QA Test Framework
 * Implements four assertion types for comprehensive validation
 * Part of AEGIS Project - OBINexus Computing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <regex.h>

/* ===================================================================
 * QA Assertion Types
 * =================================================================== */

typedef enum {
    QA_TRUE_POSITIVE,   /* Valid tokens correctly identified */
    QA_TRUE_NEGATIVE,   /* Invalid tokens correctly rejected */
    QA_FALSE_POSITIVE,  /* Invalid tokens incorrectly accepted */
    QA_FALSE_NEGATIVE   /* Valid tokens incorrectly rejected */
} QAAssertionType;

typedef struct {
    const char* test_name;
    const char* input;
    bool expected_match;
    const char* pattern;
    QAAssertionType type;
} QATestCase;

typedef struct {
    size_t total_tests;
    size_t passed_tests;
    size_t true_positives;
    size_t true_negatives;
    size_t false_positives;
    size_t false_negatives;
} QATestResults;

/* ===================================================================
 * Token Pattern Definitions for Testing
 * =================================================================== */

typedef struct {
    const char* name;
    const char* pattern;
    regex_t compiled;
    bool is_compiled;
} TestPattern;

static TestPattern test_patterns[] = {
    {"identifier", "^[a-zA-Z_][a-zA-Z0-9_]*$", {0}, false},
    {"number", "^[0-9]+(\\.[0-9]+)?$", {0}, false},
    {"operator", "^[-+*/=<>!&|^~%]$", {0}, false},
    {"quantum_init", "^@quantum", {0}, false},
    {"collapse", "^!collapse", {0}, false},
    {"governance", "^#\\[gov:.*\\]", {0}, false},
};

/* ===================================================================
 * QA Test Cases
 * =================================================================== */

static QATestCase qa_test_cases[] = {
    /* True Positive Tests - Valid tokens that should be accepted */
    {"TP_identifier_basic", "validIdentifier", true, "identifier", QA_TRUE_POSITIVE},
    {"TP_identifier_underscore", "_private_var", true, "identifier", QA_TRUE_POSITIVE},
    {"TP_identifier_mixed", "myVar123", true, "identifier", QA_TRUE_POSITIVE},
    {"TP_number_integer", "42", true, "number", QA_TRUE_POSITIVE},
    {"TP_number_decimal", "3.14159", true, "number", QA_TRUE_POSITIVE},
    {"TP_operator_plus", "+", true, "operator", QA_TRUE_POSITIVE},
    {"TP_operator_caret", "^", true, "operator", QA_TRUE_POSITIVE},
    {"TP_operator_mod", "%", true, "operator", QA_TRUE_POSITIVE},
    {"TP_quantum_marker", "@quantum", true, "quantum_init", QA_TRUE_POSITIVE},
    {"TP_collapse_marker", "!collapse", true, "collapse", QA_TRUE_POSITIVE},
    {"TP_governance_tag", "#[gov:aegis_phase_1]", true, "governance", QA_TRUE_POSITIVE},
    
    /* True Negative Tests - Invalid tokens that should be rejected */
    {"TN_identifier_start_digit", "123invalid", false, "identifier", QA_TRUE_NEGATIVE},
    {"TN_identifier_special_char", "var-name", false, "identifier", QA_TRUE_NEGATIVE},
    {"TN_identifier_space", "my var", false, "identifier", QA_TRUE_NEGATIVE},
    {"TN_number_invalid", "12.34.56", false, "number", QA_TRUE_NEGATIVE},
    {"TN_number_letter", "123abc", false, "number", QA_TRUE_NEGATIVE},
    {"TN_operator_double", "++", false, "operator", QA_TRUE_NEGATIVE},
    {"TN_operator_invalid", "@", false, "operator", QA_TRUE_NEGATIVE},
    {"TN_quantum_partial", "@quant", false, "quantum_init", QA_TRUE_NEGATIVE},
    {"TN_collapse_partial", "!coll", false, "collapse", QA_TRUE_NEGATIVE},
    {"TN_governance_malformed", "#[gov:", false, "governance", QA_TRUE_NEGATIVE},
    
    /* False Positive Tests - Edge cases that might be incorrectly accepted */
    {"FP_identifier_emoji", "var😀", false, "identifier", QA_FALSE_POSITIVE},
    {"FP_identifier_unicode", "αβγ", false, "identifier", QA_FALSE_POSITIVE},
    {"FP_number_hex", "0xFF", false, "number", QA_FALSE_POSITIVE},
    {"FP_number_scientific", "1e-10", false, "number", QA_FALSE_POSITIVE},
    {"FP_operator_composite", "=>", false, "operator", QA_FALSE_POSITIVE},
    {"FP_quantum_case", "@QUANTUM", false, "quantum_init", QA_FALSE_POSITIVE},
    
    /* False Negative Tests - Valid tokens that might be incorrectly rejected */
    {"FN_identifier_long", "very_long_identifier_name_that_is_still_valid", true, "identifier", QA_FALSE_NEGATIVE},
    {"FN_identifier_single", "x", true, "identifier", QA_FALSE_NEGATIVE},
    {"FN_number_zero", "0", true, "number", QA_FALSE_NEGATIVE},
    {"FN_number_leading_zero", "0.5", true, "number", QA_FALSE_NEGATIVE},
    {"FN_operator_tilde", "~", true, "operator", QA_FALSE_NEGATIVE},
    {"FN_governance_complex", "#[gov:stage_0_compliance_check_v2]", true, "governance", QA_FALSE_NEGATIVE},
};

/* ===================================================================
 * Pattern Compilation and Cleanup
 * =================================================================== */

bool compile_test_patterns(void) {
    size_t pattern_count = sizeof(test_patterns) / sizeof(TestPattern);
    
    for (size_t i = 0; i < pattern_count; i++) {
        int ret = regcomp(&test_patterns[i].compiled, 
                         test_patterns[i].pattern, 
                         REG_EXTENDED | REG_NOSUB);
        if (ret != 0) {
            char errbuf[256];
            regerror(ret, &test_patterns[i].compiled, errbuf, sizeof(errbuf));
            fprintf(stderr, "Failed to compile pattern '%s': %s\n", 
                   test_patterns[i].name, errbuf);
            
            /* Cleanup previously compiled patterns */
            for (size_t j = 0; j < i; j++) {
                if (test_patterns[j].is_compiled) {
                    regfree(&test_patterns[j].compiled);
                    test_patterns[j].is_compiled = false;
                }
            }
            return false;
        }
        test_patterns[i].is_compiled = true;
    }
    
    return true;
}

void cleanup_test_patterns(void) {
    size_t pattern_count = sizeof(test_patterns) / sizeof(TestPattern);
    
    for (size_t i = 0; i < pattern_count; i++) {
        if (test_patterns[i].is_compiled) {
            regfree(&test_patterns[i].compiled);
            test_patterns[i].is_compiled = false;
        }
    }
}

/* ===================================================================
 * Test Execution
 * =================================================================== */

TestPattern* find_pattern_by_name(const char* name) {
    size_t pattern_count = sizeof(test_patterns) / sizeof(TestPattern);
    
    for (size_t i = 0; i < pattern_count; i++) {
        if (strcmp(test_patterns[i].name, name) == 0) {
            return &test_patterns[i];
        }
    }
    
    return NULL;
}

bool run_single_test(const QATestCase* test, QATestResults* results) {
    TestPattern* pattern = find_pattern_by_name(test->pattern);
    if (!pattern || !pattern->is_compiled) {
        fprintf(stderr, "Pattern '%s' not found or not compiled\n", test->pattern);
        return false;
    }
    
    /* Execute regex match */
    int ret = regexec(&pattern->compiled, test->input, 0, NULL, 0);
    bool matched = (ret == 0);
    
    /* Determine test result */
    bool test_passed = false;
    const char* result_str = "";
    
    switch (test->type) {
        case QA_TRUE_POSITIVE:
            test_passed = (matched && test->expected_match);
            if (test_passed) {
                results->true_positives++;
                result_str = "TRUE POSITIVE ✓";
            } else {
                result_str = "FAILED (Expected match)";
            }
            break;
            
        case QA_TRUE_NEGATIVE:
            test_passed = (!matched && !test->expected_match);
            if (test_passed) {
                results->true_negatives++;
                result_str = "TRUE NEGATIVE ✓";
            } else {
                result_str = "FAILED (Expected no match)";
            }
            break;
            
        case QA_FALSE_POSITIVE:
            /* These tests check if the pattern incorrectly accepts invalid input */
            test_passed = !matched;  /* Pass if it correctly rejects */
            if (!matched) {
                result_str = "CORRECTLY REJECTED ✓";
            } else {
                results->false_positives++;
                result_str = "FALSE POSITIVE ✗";
            }
            break;
            
        case QA_FALSE_NEGATIVE:
            /* These tests check if the pattern incorrectly rejects valid input */
            test_passed = matched;  /* Pass if it correctly accepts */
            if (matched) {
                result_str = "CORRECTLY ACCEPTED ✓";
            } else {
                results->false_negatives++;
                result_str = "FALSE NEGATIVE ✗";
            }
            break;
    }
    
    /* Update statistics */
    results->total_tests++;
    if (test_passed) {
        results->passed_tests++;
    }
    
    /* Print result */
    printf("%-35s %-20s %-15s %s\n", 
           test->test_name, 
           test->input, 
           test->pattern,
           result_str);
    
    return test_passed;
}

/* ===================================================================
 * Report Generation
 * =================================================================== */

void print_qa_report(const QATestResults* results) {
    printf("\n");
    printf("=======================================================\n");
    printf("RIFT Stage-0 QA Test Report\n");
    printf("=======================================================\n");
    printf("Total Tests:      %zu\n", results->total_tests);
    printf("Passed Tests:     %zu (%.1f%%)\n", 
           results->passed_tests, 
           (double)results->passed_tests / results->total_tests * 100.0);
    printf("\n");
    printf("Assertion Analysis:\n");
    printf("  True Positives:  %zu (Valid tokens correctly accepted)\n", 
           results->true_positives);
    printf("  True Negatives:  %zu (Invalid tokens correctly rejected)\n", 
           results->true_negatives);
    printf("  False Positives: %zu (Invalid tokens incorrectly accepted)\n", 
           results->false_positives);
    printf("  False Negatives: %zu (Valid tokens incorrectly rejected)\n", 
           results->false_negatives);
    printf("\n");
    
    /* Calculate metrics */
    double precision = 0.0;
    double recall = 0.0;
    double f1_score = 0.0;
    
    if (results->true_positives + results->false_positives > 0) {
        precision = (double)results->true_positives / 
                   (results->true_positives + results->false_positives);
    }
    
    if (results->true_positives + results->false_negatives > 0) {
        recall = (double)results->true_positives / 
                (results->true_positives + results->false_negatives);
    }
    
    if (precision + recall > 0) {
        f1_score = 2.0 * (precision * recall) / (precision + recall);
    }
    
    printf("Performance Metrics:\n");
    printf("  Precision:       %.2f%%\n", precision * 100.0);
    printf("  Recall:          %.2f%%\n", recall * 100.0);
    printf("  F1 Score:        %.2f%%\n", f1_score * 100.0);
    printf("\n");
    
    /* AEGIS Compliance Check */
    bool aegis_compliant = (results->false_positives == 0) && 
                          (results->false_negatives == 0) &&
                          (results->passed_tests == results->total_tests);
    
    printf("AEGIS Compliance: %s\n", aegis_compliant ? "✓ PASS" : "✗ FAIL");
    if (!aegis_compliant) {
        printf("  Compliance requires 100%% pass rate with no false results\n");
    }
    printf("=======================================================\n");
}

/* ===================================================================
 * Main Test Runner
 * =================================================================== */
int main(int argc, char* argv[]) {
    printf("RIFT Stage-0 QA Test Suite\n");
    printf("AEGIS Project - OBINexus Computing\n");
    printf("=======================================================\n\n");

    /* Compile patterns */
    if (!compile_test_patterns()) {
        fprintf(stderr, "Failed to compile test patterns\n");
        return 1;
    }

    /* Initialize results */
    QATestResults results = {0};

    /* Run all tests in order, showing case name and result */
    size_t test_count = sizeof(qa_test_cases) / sizeof(QATestCase);

    printf("Running All QA Assertion Cases:\n");
    printf("-------------------------------------------------------\n");
    for (size_t i = 0; i < test_count; i++) {
        run_single_test(&qa_test_cases[i], &results);
    }

    /* Generate report */
    print_qa_report(&results);

    /* Cleanup */
    cleanup_test_patterns();

    /* Return appropriate exit code */
    return (results.passed_tests == results.total_tests) ? 0 : 1;
}
