# Modular Makefile: auto-detect modules, include dirs and sources

CXX := g++
STD := -std=c++17
CXXFLAGS := -Wall -Wextra -O3 $(STD)
PROFILE := -pg -DPROFILE
DEBUG := -DDEBUG
LDLIBS := -lSDL2

# find all include directories (any folder named "include")
INC_DIRS := $(shell find . -type d -name include 2>/dev/null | sed 's|^./||')
CPPFLAGS := $(patsubst %,-I%,$(INC_DIRS))

# collect all .cpp sources (skip build dir and tests/testbench.cpp to avoid duplicate mains)
SRCS := $(shell find . -name '*.cpp' ! -path './build/*' ! -path './tests/*' -print | sed 's|^./||')

# place objects under build/ preserving directory structure
OBJS := $(patsubst %.cpp,build/%.o,$(SRCS))
# Generate dependency files (.d) alongside object files (.o)
DEP_FILES := $(patsubst %.o,%.d,$(OBJS))

TARGET := psx

.PHONY: all clean show profile

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo Linking $@
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDLIBS)

# generic rule: compile source -> object under build/
# ensures directory exists before compiling
# -MMD -MP flags generate dependency files automatically
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

show:
	@echo "Include dirs: $(INC_DIRS)"
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "Dependency files: $(DEP_FILES)"

clean:
	@echo Cleaning build artifacts
	@rm -rf build/ $(TARGET)
	@rm -f *.out* $(DEP_FILES)

debug: CXXFLAGS += $(DEBUG)
debug: clean all

profile: CXXFLAGS += $(PROFILE)
profile: clean all
	@valgrind --tool=callgrind ./$(TARGET)
	@callgrind_annotate callgrind.out.*
# 	@kcachegrind callgrind.out.*

# Include the generated dependency files.
# This makes Make aware of header file dependencies,
# preventing unnecessary recompilation when only headers change.
-include $(DEP_FILES)