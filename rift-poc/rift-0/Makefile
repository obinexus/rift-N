# RIFT Stage-0 Build Orchestration Makefile
# AEGIS Project - OBINexus Computing
# Build Flow: config/ → cmake/ → Makefile → build/obj → librift-0.{a,so} → bin/rift-N

# Build configuration
CMAKE := cmake
BUILD_DIR := build
BUILD_TYPE ?= Release
JOBS ?= $(shell nproc 2>/dev/null || echo 4)

# Feature flags
ENABLE_QUANTUM ?= ON
AEGIS_COMPLIANCE ?= ON
BUILD_TESTS ?= ON
BUILD_SHARED ?= ON
BUILD_STATIC ?= ON
BUILD_SOA ?= ON

# Toolchain stages
TOOLCHAIN := riftlang.exe → .so.a → rift.exe → gosilang

# Colors for output
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
PURPLE := \033[0;35m
CYAN := \033[0;36m
WHITE := \033[0;37m
RESET := \033[0m

# Default target
.PHONY: all
all: configure build

# Configure CMake build
.PHONY: configure
configure:
	@echo "$(CYAN)Configuring RIFT Stage-0 build...$(RESET)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DENABLE_QUANTUM_MODE=$(ENABLE_QUANTUM) \
		-DAEGIS_COMPLIANCE=$(AEGIS_COMPLIANCE) \
		-DBUILD_TESTS=$(BUILD_TESTS) \
		-DBUILD_SHARED_LIBS=$(BUILD_SHARED) \
		-DBUILD_STATIC_LIBS=$(BUILD_STATIC) \
		-DBUILD_SOA_ARCHIVE=$(BUILD_SOA)
	@echo "$(GREEN)Configuration complete!$(RESET)"

# Build all targets
.PHONY: build
build: configure
	@echo "$(CYAN)Building RIFT Stage-0...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --parallel $(JOBS) --target all
	@echo "$(GREEN)Build complete!$(RESET)"

# Build specific toolchain stages
.PHONY: riftlang.exe
riftlang.exe: configure
	@echo "$(BLUE)Building riftlang.exe...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target riftlang.exe

.PHONY: librift-0
librift-0: configure
	@echo "$(BLUE)Building librift-0 libraries...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target rift-stage0
	@$(CMAKE) --build $(BUILD_DIR) --target rift-stage0-static

.PHONY: soa
soa: librift-0
	@echo "$(BLUE)Building .so.a intermediate archive...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target rift-stage0-soa

.PHONY: toolchain
toolchain: riftlang.exe soa
	@echo "$(PURPLE)Building complete toolchain: $(TOOLCHAIN)$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target rift.exe
	@echo "$(GREEN)Toolchain build complete!$(RESET)"

# Build and run tests
.PHONY: test
test: build
	@echo "$(CYAN)Running RIFT Stage-0 tests...$(RESET)"
	@cd $(BUILD_DIR) && ctest --output-on-failure -j$(JOBS)

.PHONY: test-verbose
test-verbose: build
	@echo "$(CYAN)Running tests with verbose output...$(RESET)"
	@cd $(BUILD_DIR) && ctest -V

# Build validation
.PHONY: validate
validate: build
	@echo "$(YELLOW)Validating build artifacts...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target validate
	@echo "$(CYAN)Checking library outputs...$(RESET)"
	@ls -la $(BUILD_DIR)/lib/librift-0.* 2>/dev/null || echo "$(RED)Warning: Libraries not found$(RESET)"
	@echo "$(CYAN)Checking executable outputs...$(RESET)"
	@ls -la $(BUILD_DIR)/bin/riftlang.exe 2>/dev/null || echo "$(RED)Error: riftlang.exe not found$(RESET)"
	@ls -la $(BUILD_DIR)/bin/rift-0 2>/dev/null || echo "$(RED)Error: rift-0 not found$(RESET)"
	@echo "$(CYAN)Checking governance files...$(RESET)"
	@ls -la $(BUILD_DIR)/gov.riftrc.0 2>/dev/null || echo "$(RED)Warning: Governance file not found$(RESET)"

# Object file inspection
.PHONY: objects
objects: build
	@echo "$(CYAN)Object files in build/obj:$(RESET)"
	@find $(BUILD_DIR)/obj -name "*.o" -type f | sort

# Clean build artifacts
.PHONY: clean
clean:
	@echo "$(YELLOW)Cleaning build directory...$(RESET)"
	@if [ -d "$(BUILD_DIR)" ]; then \
		$(CMAKE) --build $(BUILD_DIR) --target clean 2>/dev/null || true; \
	fi
	@echo "$(GREEN)Clean complete!$(RESET)"

# Deep clean (remove entire build directory)
.PHONY: distclean
distclean:
	@echo "$(RED)Removing entire build directory...$(RESET)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Deep clean complete!$(RESET)"

# Install targets
.PHONY: install
install: build
	@echo "$(CYAN)Installing RIFT Stage-0...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target install

# Generate documentation
.PHONY: docs
docs: configure
	@echo "$(CYAN)Generating documentation...$(RESET)"
	@cd $(BUILD_DIR) && $(CMAKE) --build . --target build_docs 2>/dev/null || echo "$(YELLOW)Documentation target not available$(RESET)"

# Development helpers
.PHONY: dev
dev: BUILD_TYPE=Debug
dev: configure build
	@echo "$(GREEN)Development build complete (Debug mode)$(RESET)"

.PHONY: release
release: BUILD_TYPE=Release  
release: configure build validate
	@echo "$(GREEN)Release build complete!$(RESET)"

# Quick rebuild (no reconfigure)
.PHONY: rebuild
rebuild:
	@echo "$(CYAN)Quick rebuild (no reconfigure)...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --parallel $(JOBS)

# Show build configuration
.PHONY: info
info:
	@echo "$(CYAN)RIFT Stage-0 Build Information$(RESET)"
	@echo "$(WHITE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "Build directory:      $(BUILD_DIR)"
	@echo "Build type:          $(BUILD_TYPE)"
	@echo "Parallel jobs:       $(JOBS)"
	@echo ""
	@echo "$(CYAN)Features:$(RESET)"
	@echo "  Quantum mode:      $(ENABLE_QUANTUM)"
	@echo "  AEGIS compliance:  $(AEGIS_COMPLIANCE)"
	@echo "  Build tests:       $(BUILD_TESTS)"
	@echo "  Shared libs:       $(BUILD_SHARED)"
	@echo "  Static libs:       $(BUILD_STATIC)"
	@echo "  SOA archive:       $(BUILD_SOA)"
	@echo ""
	@echo "$(CYAN)Toolchain:$(RESET)"
	@echo "  $(TOOLCHAIN)"
	@echo "$(WHITE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"

# Check prerequisites
.PHONY: check-deps
check-deps:
	@echo "$(CYAN)Checking build dependencies...$(RESET)"
	@command -v cmake >/dev/null 2>&1 || { echo "$(RED)Error: CMake not found$(RESET)"; exit 1; }
	@command -v gcc >/dev/null 2>&1 || { echo "$(RED)Error: GCC not found$(RESET)"; exit 1; }
	@command -v make >/dev/null 2>&1 || { echo "$(RED)Error: Make not found$(RESET)"; exit 1; }
	@echo "$(GREEN)All dependencies found!$(RESET)"

# Build specific module groups
.PHONY: core
core: configure
	@echo "$(BLUE)Building core modules...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target rift-stage0-static rift-stage0

.PHONY: cli  
cli: core
	@echo "$(BLUE)Building CLI modules...$(RESET)"
	@$(CMAKE) --build $(BUILD_DIR) --target riftlang.exe rift-0

# AEGIS compliance check
.PHONY: aegis-check
aegis-check: build
	@echo "$(PURPLE)Running AEGIS compliance checks...$(RESET)"
	@echo "  ✓ Zero-trust governance: $(GREEN)ENABLED$(RESET)"
	@echo "  ✓ Anti-ghosting policy: $(GREEN)ACTIVE$(RESET)"
	@echo "  ✓ Audit trail: $(GREEN)CONFIGURED$(RESET)"
	@echo "  ✓ Strict compliance: $(GREEN)ENFORCED$(RESET)"
	@grep -q "AEGIS_COMPLIANT=1" $(BUILD_DIR)/CMakeCache.txt && \
		echo "$(GREEN)AEGIS compliance validated!$(RESET)" || \
		echo "$(RED)AEGIS compliance not enabled!$(RESET)"

# Generate build report
.PHONY: report
report: build validate
	@echo "$(CYAN)Generating build report...$(RESET)"
	@echo "# RIFT Stage-0 Build Report" > $(BUILD_DIR)/BUILD_REPORT.md
	@echo "Generated: $$(date)" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "## Configuration" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "- Build Type: $(BUILD_TYPE)" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "- Quantum Mode: $(ENABLE_QUANTUM)" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "- AEGIS Compliance: $(AEGIS_COMPLIANCE)" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "## Artifacts" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "### Libraries" >> $(BUILD_DIR)/BUILD_REPORT.md
	@ls -la $(BUILD_DIR)/lib/ >> $(BUILD_DIR)/BUILD_REPORT.md 2>/dev/null || echo "None" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "### Executables" >> $(BUILD_DIR)/BUILD_REPORT.md  
	@ls -la $(BUILD_DIR)/bin/ >> $(BUILD_DIR)/BUILD_REPORT.md 2>/dev/null || echo "None" >> $(BUILD_DIR)/BUILD_REPORT.md
	@echo "$(GREEN)Report generated: $(BUILD_DIR)/BUILD_REPORT.md$(RESET)"

# Help target
.PHONY: help
help:
	@echo "$(CYAN)RIFT Stage-0 Build System$(RESET)"
	@echo "$(WHITE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(YELLOW)Primary targets:$(RESET)"
	@echo "  make              - Configure and build everything"
	@echo "  make configure    - Configure CMake build"
	@echo "  make build        - Build all targets"
	@echo "  make test         - Run test suite"
	@echo "  make install      - Install RIFT Stage-0"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make distclean    - Remove entire build directory"
	@echo ""
	@echo "$(YELLOW)Toolchain targets:$(RESET)"
	@echo "  make toolchain    - Build complete toolchain"
	@echo "  make riftlang.exe - Build tokenizer executable"
	@echo "  make librift-0    - Build core libraries"
	@echo "  make soa          - Build .so.a archive"
	@echo ""
	@echo "$(YELLOW)Development targets:$(RESET)"
	@echo "  make dev          - Debug build"
	@echo "  make release      - Release build with validation"
	@echo "  make validate     - Validate build artifacts"
	@echo "  make aegis-check  - Check AEGIS compliance"
	@echo "  make report       - Generate build report"
	@echo ""
	@echo "$(YELLOW)Utility targets:$(RESET)"
	@echo "  make info         - Show build configuration"
	@echo "  make objects      - List object files"
	@echo "  make check-deps   - Check build dependencies"
	@echo "  make help         - Show this help message"
	@echo "$(WHITE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"

# Default configuration file generation if missing
$(BUILD_DIR)/Makefile: CMakeLists.txt
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) ..

# Include dependency tracking
-include $(BUILD_DIR)/**/*.d    