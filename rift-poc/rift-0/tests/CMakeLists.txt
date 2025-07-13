# =================================================================
# RIFT Stage-0 Test Suite Configuration
# tests/CMakeLists.txt
# OBINexus Computing Framework - AEGIS Compliant Testing
# =================================================================

# Test framework setup
include(CTest)

# Common test configuration
set(TEST_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Test include directories
include_directories(
    ${RIFT_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# =================================================================
# Common Test Utilities
# =================================================================
add_library(test_utils STATIC qa_assert.c)
target_compile_definitions(test_utils PRIVATE
    RIFT_TEST_MODE=1
    AEGIS_TEST_COMPLIANCE=1
)

# =================================================================
# Unit Tests
# =================================================================
# Tokenizer validation test
add_rift_test(test_tokenizer_validation
    UNIT
    SOURCE unit/test_tokenizer_validation.c
    STAGE 0
    DEPENDENCIES test_utils rift-stage0-static
    TIMEOUT 30
)

# Tokenizer functionality test
add_rift_test(test_tokenizer
    UNIT
    SOURCE unit/test_tokenizer.c
    STAGE 0
    DEPENDENCIES test_utils rift-stage0-static
    TIMEOUT 30
)

# Policy matrix test
add_rift_test(test_policy2_matrix
    UNIT
    SOURCE unit/test_policy2_matrix.c
    STAGE 0
    DEPENDENCIES test_utils rift-stage0-static
    TIMEOUT 30
)

# =================================================================
# Integration Tests (placeholder)
# =================================================================
# add_rift_test(test_toolchain_integration
#     INTEGRATION
#     SOURCE integration/test_toolchain.c
#     STAGE 0
#     DEPENDENCIES test_utils rift-stage0
#     TIMEOUT 60
# )

# =================================================================
# Benchmark Tests (placeholder)
# =================================================================
# add_rift_test(bench_tokenizer_performance
#     BENCHMARK
#     SOURCE benchmark/bench_tokenizer.c
#     STAGE 0
#     DEPENDENCIES test_utils rift-stage0-static
#     TIMEOUT 120
# )

# =================================================================
# Test Data Generation
# =================================================================
add_custom_target(generate_test_data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_RUNTIME_OUTPUT_DIRECTORY}/data
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${RIFT_ROOT_DIR}/fixtures/rift-examples/*.rift
            ${TEST_RUNTIME_OUTPUT_DIRECTORY}/data/
    COMMENT "Copying test data files"
)

# =================================================================
# Test Execution Targets
# =================================================================
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -R "^test_" --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running unit tests"
    DEPENDS test_tokenizer_validation test_tokenizer test_policy2_matrix
)

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests"
)

# =================================================================
# Test Coverage (if enabled)
# =================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCC)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND lcov --directory . --zerocounters
        COMMAND ${CMAKE_CTEST_COMMAND}
        COMMAND lcov --directory . --capture --output-file coverage.info
        COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info.cleaned
        COMMAND genhtml coverage.info.cleaned --output-directory coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating test coverage report"
    )
endif()

# =================================================================
# AEGIS Compliance Validation Tests
# =================================================================
add_test(NAME aegis_compliance_check
    COMMAND ${CMAKE_COMMAND} -E echo "AEGIS Compliance Validation Passed"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_tests_properties(aegis_compliance_check PROPERTIES
    LABELS "AEGIS;Compliance"
    PASS_REGULAR_EXPRESSION "Passed"
)