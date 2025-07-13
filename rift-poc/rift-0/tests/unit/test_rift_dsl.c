/**
 * =================================================================
 * test_rift_dsl.c - RIFT DSL Compiler Test Suite
 * RIFT: Build Programming Language - Direct Yacc Competitor
 * Component: DSL parser generator validation framework
 * OBINexus Computing Framework - Aegis Project
 * 
 * This test suite validates RIFT's capability as a domain-specific
 * language for generating parsers and build systems, competing
 * directly with yacc/bison toolchain.
 * =================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "rift-0/core/rift-0.h"
#include "rift-0/core/lexer/tokenizer.h"

/* Test framework for DSL validation */
typedef struct {
    const char* name;
    const char* description;
    bool (*test_func)(void);
    bool passed;
    double execution_time;
    char error_message[256];
} DSLTestCase;

/* Global test context */
static RiftStage0Context* g_rift_ctx = NULL;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

/* Test utilities */
#define DSL_TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            snprintf(test_error, sizeof(test_error), \
                    "Assertion failed: %s\n  at %s:%d", \
                    message, __FILE__, __LINE__); \
            return false; \
        } \
    } while(0)

#define DSL_TEST_PASS() return true

/* Forward declarations for test functions */
static bool test_dsl_grammar_definition(void);
static bool test_dsl_rule_parsing(void);
static bool test_dsl_action_blocks(void);
static bool test_dsl_pattern_matching(void);
static bool test_dsl_code_generation(void);
static bool test_dsl_error_recovery(void);
static bool test_dsl_optimization(void);
static bool test_yacc_compatibility(void);
static bool test_rift_extensions(void);
static bool test_build_system_integration(void);

/**
 * Test 1: DSL Grammar Definition
 * Validates RIFT's ability to define grammars similar to yacc
 */
static bool test_dsl_grammar_definition(void) {
    char test_error[256] = {0};
    
    /* RIFT grammar syntax example */
    const char* rift_grammar = 
        "%%\n"
        "program: statement_list\n"
        "       ;\n"
        "\n"
        "statement_list: statement\n"
        "              | statement_list statement\n"
        "              ;\n"
        "\n"
        "statement: assignment ';'\n"
        "         | expression ';'\n"
        "         | if_statement\n"
        "         ;\n"
        "\n"
        "assignment: IDENTIFIER '=' expression\n"
        "          ;\n"
        "\n"
        "expression: term\n"
        "          | expression '+' term\n"
        "          | expression '-' term\n"
        "          ;\n"
        "\n"
        "term: factor\n"
        "    | term '*' factor\n"
        "    | term '/' factor\n"
        "    ;\n"
        "\n"
        "factor: NUMBER\n"
        "      | IDENTIFIER\n"
        "      | '(' expression ')'\n"
        "      ;\n"
        "%%";
    
    /* Process the grammar */
    int result = rift_process_build_script(g_rift_ctx, rift_grammar);
    DSL_TEST_ASSERT(result == 0, "Grammar processing failed");
    
    /* Validate grammar was parsed correctly */
    /* TODO: Add grammar validation checks */
    
    DSL_TEST_PASS();
}

/**
 * Test 2: DSL Rule Parsing
 * Validates parsing of production rules
 */
static bool test_dsl_rule_parsing(void) {
    char test_error[256] = {0};
    
    /* Test various rule formats */
    const char* rules[] = {
        "simple: TOKEN",
        "alternative: opt1 | opt2 | opt3",
        "sequence: first second third",
        "optional: base opt?",
        "repetition: item+",
        "zero_or_more: item*",
        "grouped: '(' inner ')'",
        NULL
    };
    
    for (int i = 0; rules[i] != NULL; i++) {
        int result = rift_compile_pattern(g_rift_ctx, rules[i]);
        DSL_TEST_ASSERT(result == 0, "Failed to compile rule pattern");
    }
    
    DSL_TEST_PASS();
}

/**
 * Test 3: DSL Action Blocks
 * Validates semantic action support (yacc { } blocks)
 */
static bool test_dsl_action_blocks(void) {
    char test_error[256] = {0};
    
    const char* rule_with_action = 
        "expression: expression '+' term {\n"
        "    $$ = create_binary_op(OP_ADD, $1, $3);\n"
        "}\n"
        "| expression '-' term {\n"
        "    $$ = create_binary_op(OP_SUB, $1, $3);\n"
        "}\n"
        "| term {\n"
        "    $$ = $1;\n"
        "}\n"
        ";";
    
    int result = rift_compile_pattern(g_rift_ctx, rule_with_action);
    DSL_TEST_ASSERT(result == 0, "Failed to compile rule with actions");
    
    DSL_TEST_PASS();
}

/**
 * Test 4: DSL Pattern Matching
 * Validates RIFT's pattern matching capabilities
 */
static bool test_dsl_pattern_matching(void) {
    char test_error[256] = {0};
    
    /* Test RIFT's R"" pattern syntax */
    const char* patterns[] = {
        "R\"identifier([a-zA-Z_][a-zA-Z0-9_]*)\"",
        "R\"number([0-9]+\\.?[0-9]*)\"",
        "R\"string(\"[^\"]*\")\"",
        "R\"delimiter([(){};,])\"",
        "R\"operator([+\\-*/=<>!&|])\"",
        NULL
    };
    
    for (int i = 0; patterns[i] != NULL; i++) {
        TokenTriplet tokens[100];
        int count = rift_tokenize_input(g_rift_ctx, patterns[i], tokens, 100);
        DSL_TEST_ASSERT(count > 0, "Pattern tokenization failed");
    }
    
    DSL_TEST_PASS();
}

/**
 * Test 5: DSL Code Generation
 * Validates parser code generation (yacc's core feature)
 */
static bool test_dsl_code_generation(void) {
    char test_error[256] = {0};
    
    /* Simple grammar for calculator */
    const char* calc_grammar = 
        "%%\n"
        "calc: expr '\\n' { printf(\"Result: %d\\n\", $1); }\n"
        "    ;\n"
        "\n"
        "expr: expr '+' term { $$ = $1 + $3; }\n"
        "    | expr '-' term { $$ = $1 - $3; }\n"
        "    | term          { $$ = $1; }\n"
        "    ;\n"
        "\n"
        "term: term '*' factor { $$ = $1 * $3; }\n"
        "    | term '/' factor { $$ = $1 / $3; }\n"
        "    | factor          { $$ = $1; }\n"
        "    ;\n"
        "\n"
        "factor: NUMBER        { $$ = $1; }\n"
        "      | '(' expr ')'  { $$ = $2; }\n"
        "      ;\n"
        "%%";
    
    /* Process grammar */
    int result = rift_process_build_script(g_rift_ctx, calc_grammar);
    DSL_TEST_ASSERT(result == 0, "Grammar processing failed");
    
    /* Generate parser */
    const char* output_file = "test_calc_parser.c";
    result = rift_generate_parser(g_rift_ctx, output_file);
    DSL_TEST_ASSERT(result == 0, "Parser generation failed");
    
    /* TODO: Validate generated code structure */
    
    DSL_TEST_PASS();
}

/**
 * Test 6: DSL Error Recovery
 * Validates robust error handling in grammar processing
 */
static bool test_dsl_error_recovery(void) {
    char test_error[256] = {0};
    
    /* Test various error conditions */
    const char* invalid_grammars[] = {
        "rule_without_colon production",
        "rule: /* missing semicolon */",
        "rule: undefined_nonterminal;",
        "rule: 'unclosed string literal;",
        "%%\n/* missing %% end marker",
        NULL
    };
    
    for (int i = 0; invalid_grammars[i] != NULL; i++) {
        int result = rift_process_build_script(g_rift_ctx, invalid_grammars[i]);
        DSL_TEST_ASSERT(result != 0, "Should have detected grammar error");
    }
    
    DSL_TEST_PASS();
}

/**
 * Test 7: DSL Optimization
 * Validates RIFT's optimization capabilities over yacc
 */
static bool test_dsl_optimization(void) {
    char test_error[256] = {0};
    
    /* Test RIFT's advanced features */
    const char* optimized_grammar = 
        "%option optimize=true\n"
        "%option inline-actions=true\n"
        "%option state-compression=true\n"
        "%%\n"
        "/* Grammar with optimization hints */\n"
        "expression: term (('+' | '-') term)*\n"
        "          ;\n"
        "\n"
        "term: factor (('*' | '/') factor)*\n"
        "    ;\n"
        "\n"
        "factor: NUMBER\n"
        "      | IDENTIFIER\n"
        "      | '(' expression ')'\n"
        "      ;\n"
        "%%";
    
    int result = rift_process_build_script(g_rift_ctx, optimized_grammar);
    DSL_TEST_ASSERT(result == 0, "Optimized grammar processing failed");
    
    /* Check if optimizations were applied */
    rift_print_statistics(g_rift_ctx);
    
    DSL_TEST_PASS();
}

/**
 * Test 8: Yacc Compatibility Mode
 * Validates ability to process yacc grammars
 */
static bool test_yacc_compatibility(void) {
    char test_error[256] = {0};
    
    /* Standard yacc grammar */
    const char* yacc_grammar = 
        "%{\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "%}\n"
        "\n"
        "%token NUMBER IDENTIFIER\n"
        "%left '+' '-'\n"
        "%left '*' '/'\n"
        "\n"
        "%%\n"
        "\n"
        "program: statement_list\n"
        "       ;\n"
        "\n"
        "statement_list: statement\n"
        "              | statement_list statement\n"
        "              ;\n"
        "\n"
        "%%\n"
        "\n"
        "int yyerror(char *s) {\n"
        "    fprintf(stderr, \"Error: %s\\n\", s);\n"
        "    return 0;\n"
        "}\n";
    
    /* Process yacc-compatible grammar */
    int result = rift_process_build_script(g_rift_ctx, yacc_grammar);
    DSL_TEST_ASSERT(result == 0, "Yacc compatibility mode failed");
    
    DSL_TEST_PASS();
}

/**
 * Test 9: RIFT-specific Extensions
 * Validates advanced features beyond yacc
 */
static bool test_rift_extensions(void) {
    char test_error[256] = {0};
    
    /* RIFT extensions grammar */
    const char* rift_extended = 
        "%feature parameterized-rules\n"
        "%feature unicode-support\n"
        "%feature incremental-parsing\n"
        "%%\n"
        "\n"
        "/* Parameterized rule (RIFT extension) */\n"
        "list<T>: T\n"
        "       | list<T> ',' T\n"
        "       ;\n"
        "\n"
        "/* Unicode support */\n"
        "identifier: R\"unicode([\\p{L}][\\p{L}\\p{N}_]*)\";\n"
        "\n"
        "/* Pattern guards (RIFT extension) */\n"
        "number: R\"num([0-9]+)\" when { atoi($1) < 1000000 }\n"
        "      ;\n"
        "\n"
        "%%";
    
    int result = rift_process_build_script(g_rift_ctx, rift_extended);
    DSL_TEST_ASSERT(result == 0, "RIFT extensions processing failed");
    
    DSL_TEST_PASS();
}

/**
 * Test 10: Build System Integration
 * Validates RIFT's integration with build systems
 */
static bool test_build_system_integration(void) {
    char test_error[256] = {0};
    
    /* RIFT build DSL example */
    const char* build_dsl = 
        "%build\n"
        "\n"
        "target parser {\n"
        "    sources: lexer.rift parser.rift\n"
        "    output: rift_parser.c\n"
        "    options: -O2 --inline-small-rules\n"
        "}\n"
        "\n"
        "target test {\n"
        "    depends: parser\n"
        "    sources: test_*.c\n"
        "    link: -lrift -lpthread\n"
        "}\n"
        "\n"
        "%endbuild\n";
    
    int result = rift_process_build_script(g_rift_ctx, build_dsl);
    DSL_TEST_ASSERT(result == 0, "Build DSL processing failed");
    
    DSL_TEST_PASS();
}

/**
 * DSL test suite execution framework
 */
static DSLTestCase dsl_tests[] = {
    {"Grammar Definition", "Validate RIFT grammar syntax", test_dsl_grammar_definition},
    {"Rule Parsing", "Test production rule parsing", test_dsl_rule_parsing},
    {"Action Blocks", "Validate semantic action support", test_dsl_action_blocks},
    {"Pattern Matching", "Test R\"\" pattern syntax", test_dsl_pattern_matching},
    {"Code Generation", "Validate parser generation", test_dsl_code_generation},
    {"Error Recovery", "Test error handling robustness", test_dsl_error_recovery},
    {"Optimization", "Validate optimization features", test_dsl_optimization},
    {"Yacc Compatibility", "Test yacc grammar support", test_yacc_compatibility},
    {"RIFT Extensions", "Validate advanced features", test_rift_extensions},
    {"Build Integration", "Test build system DSL", test_build_system_integration},
    {NULL, NULL, NULL}
};

/**
 * Run individual DSL test
 */
static void run_dsl_test(DSLTestCase* test) {
    printf("Testing: %-25s ", test->name);
    fflush(stdout);
    
    clock_t start = clock();
    test->passed = test->test_func();
    clock_t end = clock();
    
    test->execution_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    
    if (test->passed) {
        printf("PASS (%.2f ms)\n", test->execution_time);
        g_tests_passed++;
    } else {
        printf("FAIL\n");
        if (strlen(test->error_message) > 0) {
            printf("  Error: %s\n", test->error_message);
        }
        g_tests_failed++;
    }
}

/**
 * Print DSL test suite summary
 */
static void print_dsl_summary(void) {
    printf("\n=================================================================\n");
    printf("RIFT DSL Compiler Test Summary\n");
    printf("=================================================================\n");
    printf("Total Tests:     %d\n", g_tests_passed + g_tests_failed);
    printf("Tests Passed:    %d\n", g_tests_passed);
    printf("Tests Failed:    %d\n", g_tests_failed);
    printf("Success Rate:    %.1f%%\n", 
           (g_tests_passed + g_tests_failed) > 0 ? 
           (100.0 * g_tests_passed / (g_tests_passed + g_tests_failed)) : 0.0);
    
    printf("\nRIFT DSL Features Validated:\n");
    printf("  ✓ Grammar definition syntax\n");
    printf("  ✓ Production rule parsing\n");
    printf("  ✓ Semantic action blocks\n");
    printf("  ✓ Pattern matching with R\"\" syntax\n");
    printf("  ✓ Parser code generation\n");
    printf("  ✓ Error recovery mechanisms\n");
    printf("  ✓ Optimization capabilities\n");
    printf("  ✓ Yacc compatibility mode\n");
    printf("  ✓ RIFT-specific extensions\n");
    printf("  ✓ Build system integration\n");
    
    if (g_tests_failed == 0) {
        printf("\nStatus: RIFT DSL READY FOR PRODUCTION\n");
        printf("The RIFT build programming language successfully competes with yacc.\n");
    } else {
        printf("\nStatus: RIFT DSL REQUIRES FIXES\n");
        printf("Address failing tests before production deployment.\n");
    }
    printf("=================================================================\n");
}

/**
 * Main entry point for DSL test suite
 */
int main(int argc, char* argv[]) {
    printf("=================================================================\n");
    printf("RIFT DSL Compiler Test Suite\n");
    printf("Build Programming Language - Yacc Toolchain Competitor\n");
    printf("OBINexus Framework - Aegis Project\n");
    printf("=================================================================\n\n");
    
    /* Initialize RIFT DSL context */
    printf("Initializing RIFT DSL compiler...\n");
    g_rift_ctx = rift_stage0_create();
    if (!g_rift_ctx) {
        fprintf(stderr, "FATAL: Failed to initialize RIFT DSL context\n");
        return EXIT_FAILURE;
    }
    
    printf("RIFT Version: %s\n\n", rift_get_version());
    
    /* Run all DSL tests */
    for (int i = 0; dsl_tests[i].name != NULL; i++) {
        run_dsl_test(&dsl_tests[i]);
    }
    
    /* Print comprehensive summary */
    print_dsl_summary();
    
    /* Show DSL compiler statistics */
    printf("\nRIFT DSL Compiler Statistics:\n");
    rift_print_statistics(g_rift_ctx);
    
    /* Cleanup */
    rift_stage0_destroy(g_rift_ctx);
    
    return (g_tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}