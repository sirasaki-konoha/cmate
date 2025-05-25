
# Directory settings
OBJDIR     := deps/object
BINDIR     := bin
# Default GOAL
.DEFAULT_GOAL := all

# List of all binaries (should be set by the generator)
BINARIES   := $(addprefix $(BINDIR)/, $(PROJECT_NAMES))

# Progress tracking files
PROGRESS_DIR := .build_progress
PROGRESS_LOCK := $(PROGRESS_DIR)/lock
PROGRESS_COUNTER := $(PROGRESS_DIR)/counter
PROGRESS_TOTAL := $(PROGRESS_DIR)/total
PROGRESS_CURRENT := $(PROGRESS_DIR)/current
PROGRESS_LOG := $(PROGRESS_DIR)/log

# Initialize progress tracking
$(shell mkdir -p $(PROGRESS_DIR))

# Count total source files for all projects
TOTAL_SOURCES := $(foreach bin,$(PROJECT_NAMES),$(foreach dir,$(SRCDIRS_$(bin)),$(wildcard $(dir)/*.c $(dir)/*/*.c)) $(EXTRA_SOURCES_$(bin)))
TOTAL_COUNT := $(words $(TOTAL_SOURCES))

# Progress update function (thread-safe with file locking)
define update_progress
@( \
  exec 200>$(PROGRESS_LOCK); \
  flock 200; \
  if [ -f $(PROGRESS_COUNTER) ]; then \
    CURRENT=$$(cat $(PROGRESS_COUNTER)); \
  else \
    CURRENT=0; \
  fi; \
  CURRENT=$$((CURRENT + 1)); \
  echo $$CURRENT > $(PROGRESS_COUNTER); \
  echo "$(TOTAL_COUNT)" > $(PROGRESS_TOTAL); \
  echo "$(1)" > $(PROGRESS_CURRENT); \
  echo "$$(date '+%H:%M:%S') [$$CURRENT/$(TOTAL_COUNT)] $(1)" >> $(PROGRESS_LOG); \
  PERCENT=$$((CURRENT * 100 / $(TOTAL_COUNT))); \
  BAR_WIDTH=30; \
  FILLED=$$((PERCENT * BAR_WIDTH / 100)); \
  BAR=""; \
  for i in $$(seq 1 $$FILLED); do BAR="$$BAR█"; done; \
  for i in $$(seq $$((FILLED + 1)) $$BAR_WIDTH); do BAR="$$BAR░"; done; \
  FILENAME=$$(basename "$(1)"); \
  printf "\r\033[K\033[32m%s\033[0m \033[36m[%3d%%]\033[0m %s \033[90m%s\033[0m" \
    "$$BAR" "$$PERCENT" "$$FILENAME" "$(1)"; \
  if [ $$CURRENT -eq $(TOTAL_COUNT) ]; then \
    printf "\n"; \
  fi \
) 200>/dev/null
endef

# Linking progress function
define show_linking_progress
@( \
  exec 200>$(PROGRESS_LOCK); \
  flock 200; \
  printf "\r\033[K🔗 \033[33mLinking:\033[0m \033[1m$(1)\033[0m\n"; \
) 200>/dev/null
endef

# Build completion function
define show_build_complete
@( \
  exec 200>$(PROGRESS_LOCK); \
  flock 200; \
  if [ -f $(PROGRESS_COUNTER) ]; then \
    BUILT_COUNT=$$(cat $(PROGRESS_COUNTER)); \
    printf "\r\033[K✅ \033[32mBuild completed:\033[0m \033[1m$(1)\033[0m \033[90m($$BUILT_COUNT files)\033[0m\n"; \
  else \
    printf "\r\033[K✅ \033[32mBuild completed:\033[0m \033[1m$(1)\033[0m\n"; \
  fi \
) 200>/dev/null
endef

# Initialize progress tracking
define init_progress
@mkdir -p $(PROGRESS_DIR)
@echo "0" > $(PROGRESS_COUNTER)
@echo "$(TOTAL_COUNT)" > $(PROGRESS_TOTAL)
@echo "" > $(PROGRESS_CURRENT)
@: > $(PROGRESS_LOG)
@printf "🚀 \033[36mStarting build...\033[0m \033[90m($(TOTAL_COUNT) files total)\033[0m\n"
endef

# Progress monitoring background task (optional)
define start_progress_monitor
@( \
  while [ ! -f $(PROGRESS_DIR)/done ]; do \
    if [ -f $(PROGRESS_COUNTER) ] && [ -f $(PROGRESS_CURRENT) ]; then \
      CURRENT=$$(cat $(PROGRESS_COUNTER) 2>/dev/null || echo 0); \
      CURRENT_FILE=$$(cat $(PROGRESS_CURRENT) 2>/dev/null || echo ""); \
      if [ "$$CURRENT_FILE" != "" ] && [ $$CURRENT -lt $(TOTAL_COUNT) ]; then \
        PERCENT=$$((CURRENT * 100 / $(TOTAL_COUNT))); \
        printf "\033]0;Build Progress [%d%%] - %s\007" "$$PERCENT" "$$(basename "$$CURRENT_FILE")"; \
      fi; \
    fi; \
    sleep 0.5; \
  done \
) &
endef

# Auto-detect source files for each binary
define GEN_BINARY_RULES
# ===== $(1) =====
# Variables for $(1)
SRCDIRS_$(1)    := $$(SRCDIRS_$(1))
INCLUDE_DIRS_$(1) := $$(INCLUDE_DIRS_$(1))
EXTRA_SOURCES_$(1) := $$(EXTRA_SOURCES_$(1))
LDLIBS_$(1)     := $$(LDLIBS_$(1))
CFLAGS_$(1)     := $$(CFLAGS_$(1))
CC_$(1)         := $$(CC_$(1))
SOURCES_$(1)    := $$(foreach dir,$$(SRCDIRS_$(1)),$$(wildcard $$(dir)/*.c $$(dir)/*/*.c))
SOURCES_$(1)    := $$(SOURCES_$(1)) $$(EXTRA_SOURCES_$(1))
OBJECTS_$(1)    := $$(patsubst %.c,$$(OBJDIR)/$(1)/%.o,$$(SOURCES_$(1)))
INCLUDES_$(1)   := $$(addprefix -I,$$(INCLUDE_DIRS_$(1)))
CFLAGS_$(1)     += $$(INCLUDES_$(1))

# Build rule for $(1)
$$(BINDIR)/$(1): $$(OBJECTS_$(1)) | $$(BINDIR)
	$$(call show_linking_progress,$(1))
	@$$(CC_$(1)) $$(OBJECTS_$(1)) -o $$@ $$(LDLIBS_$(1))

# Object file rule for $(1) - with progress tracking
$$(OBJDIR)/$(1)/%.o: %.c
	@mkdir -p $$(dir $$@)
	$$(call update_progress,$$<)
	@$$(CC_$(1)) $$(CFLAGS_$(1)) -c $$< -o $$@ 2>&1 | \
	  while IFS= read -r line; do \
	    printf "\r\033[K\033[31mError in %s:\033[0m $$line\n" "$$<"; \
	  done; \
	  exit $${PIPESTATUS[0]}

# Ensure directory exists (order-only prerequisite)
$$(BINDIR):
	@mkdir -p $$(BINDIR)
endef

# Generate rules for each binary
$(foreach bin,$(PROJECT_NAMES),$(eval $(call GEN_BINARY_RULES,$(bin))))

# Default targets
.PHONY: all clean rebuild help info parallel-info watch-progress

# Main build target with progress initialization
all:
	@$(call init_progress)
	@$(MAKE) --no-print-directory build-all
	@$(call show_build_complete,$(BINARIES))
	@touch $(PROGRESS_DIR)/done

# Internal build target
build-all: $(BINARIES)

clean:
	@printf "🧹 \033[33mCleaning up...\033[0m\n"
	@rm -rf $(OBJDIR) $(BINDIR) deps $(PROGRESS_DIR)
	@printf "✅ \033[32mCleanup completed\033[0m\n"

rebuild: clean all

# Parallel build targets with progress
parallel:
	@$(call init_progress)
	@$(MAKE) -j$(shell nproc) --no-print-directory build-all
	@$(call show_build_complete,$(BINARIES))
	@touch $(PROGRESS_DIR)/done

parallel4:
	@$(call init_progress)
	@$(MAKE) -j4 --no-print-directory build-all
	@$(call show_build_complete,$(BINARIES))
	@touch $(PROGRESS_DIR)/done

parallel8:
	@$(call init_progress)
	@$(MAKE) -j8 --no-print-directory build-all
	@$(call show_build_complete,$(BINARIES))
	@touch $(PROGRESS_DIR)/done

# Watch build progress in separate terminal
watch-progress:
	@echo "Watching build progress... (Press Ctrl+C to stop)"
	@while true; do \
	  if [ -f $(PROGRESS_COUNTER) ] && [ -f $(PROGRESS_TOTAL) ]; then \
	    CURRENT=$$(cat $(PROGRESS_COUNTER) 2>/dev/null || echo 0); \
	    TOTAL=$$(cat $(PROGRESS_TOTAL) 2>/dev/null || echo 1); \
	    CURRENT_FILE=$$(cat $(PROGRESS_CURRENT) 2>/dev/null || echo "Waiting..."); \
	    PERCENT=$$((CURRENT * 100 / TOTAL)); \
	    clear; \
	    echo "=================== BUILD PROGRESS ==================="; \
	    echo "Files: $$CURRENT / $$TOTAL ($$PERCENT%)"; \
	    echo "Current: $$CURRENT_FILE"; \
	    echo "======================================================"; \
	    if [ -f $(PROGRESS_LOG) ]; then \
	      echo "Recent activity:"; \
	      tail -10 $(PROGRESS_LOG); \
	    fi; \
	    if [ -f $(PROGRESS_DIR)/done ]; then \
	      echo "✅ Build completed!"; \
	      break; \
	    fi; \
	  else \
	    echo "No build in progress..."; \
	  fi; \
	  sleep 1; \
	done

# Help
help:
	@echo "Available targets:"
	@echo "  all           - Build all binaries (default)"
	@echo "  clean         - Remove generated files"
	@echo "  rebuild       - Clean and then build all"
	@echo "  parallel      - Build with maximum parallel jobs"
	@echo "  parallel4     - Build with 4 parallel jobs"
	@echo "  parallel8     - Build with 8 parallel jobs"
	@echo "  watch-progress - Watch build progress in real-time"
	@echo "  help          - Show this help"
	@echo "  parallel-info - Show parallel build information"
	@echo ""
	@echo "Per-binary targets:"
	@echo "  $(PROJECT_NAMES)"
	@echo ""
	@echo "Usage examples:"
	@echo "  make parallel       # Parallel build with progress bar"
	@echo "  make -j4 all       # 4 parallel jobs with progress"
	@echo "  make watch-progress # Monitor progress in separate terminal"

# Show project structure
info:
	@echo "Project Structure:"
	@echo "  Total source files: $(TOTAL_COUNT)"
	@echo "  Source Dirs (per binary):"
	@$(foreach bin,$(PROJECT_NAMES),echo "    $(bin): $$(SRCDIRS_$(bin))";)
	@echo "  Include Dirs (per binary):"
	@$(foreach bin,$(PROJECT_NAMES),echo "    $(bin): $$(INCLUDE_DIRS_$(bin))";)
	@echo "  Binary outputs: $(BINARIES)"
	@echo "  Project names: $(PROJECT_NAMES)"

# Show parallel build information
parallel-info:
	@echo "Parallel Build Information:"
	@echo "  Available CPU cores: $(shell nproc)"
	@echo "  Total source files: $(TOTAL_COUNT)"
	@echo "  Progress tracking: ENABLED (cargo-style)"
	@echo "  Features:"
	@echo "    - Thread-safe progress bar with file locking"
	@echo "    - Real-time file compilation display"
	@echo "    - Color-coded output with Unicode progress bar"
	@echo "    - Terminal title updates"
	@echo "    - Build log tracking"
	@echo ""
	@echo "Usage examples:"
	@echo "  make parallel           # Use all CPU cores"
	@echo "  make -j4 all           # Use 4 cores"
	@echo "  make watch-progress &   # Monitor in background"
