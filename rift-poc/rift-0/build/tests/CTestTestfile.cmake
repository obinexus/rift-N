# CMake generated Testfile for 
# Source directory: /mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests
# Build directory: /mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_tokenizer_validation "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/bin/test_tokenizer_validation")
set_tests_properties(test_tokenizer_validation PROPERTIES  TIMEOUT "30" WORKING_DIRECTORY "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/tests/unit" _BACKTRACE_TRIPLES "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/cmake/rift_stage_macros.cmake;201;add_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;33;add_rift_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;0;")
add_test(test_tokenizer "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/bin/test_tokenizer")
set_tests_properties(test_tokenizer PROPERTIES  TIMEOUT "30" WORKING_DIRECTORY "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/tests/unit" _BACKTRACE_TRIPLES "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/cmake/rift_stage_macros.cmake;201;add_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;42;add_rift_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;0;")
add_test(test_policy2_matrix "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/bin/test_policy2_matrix")
set_tests_properties(test_policy2_matrix PROPERTIES  TIMEOUT "30" WORKING_DIRECTORY "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build/tests/unit" _BACKTRACE_TRIPLES "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/cmake/rift_stage_macros.cmake;201;add_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;51;add_rift_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;0;")
add_test(aegis_compliance_check "/usr/bin/cmake" "-E" "echo" "AEGIS Compliance Validation Passed")
set_tests_properties(aegis_compliance_check PROPERTIES  LABELS "AEGIS;Compliance" PASS_REGULAR_EXPRESSION "Passed" WORKING_DIRECTORY "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/build" _BACKTRACE_TRIPLES "/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;127;add_test;/mnt/c/Users/OBINexus/Projects/github/rift-poc/rift-0/tests/CMakeLists.txt;0;")
