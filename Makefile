CC := g++
TARGET := libmdalyzer
CC_FLAGS := -fPIC -Wall -Wextra -pedantic
LD_FLAGS := -lboost_program_options-mt

MODULES := computes extern frames python trajectory
SRC_DIR := $(addprefix $(TARGET)/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
OBJ := $(patsubst $(TARGET)/%.cc,build/%.o,$(SRC))
INCLUDES := $(addprefix -I,$(SRC_DIR))

vpath %.cc $(SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CC) $(CC_FLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs build/$(TARGET).so
build/$(TARGET).so: $(OBJ)
	$(CC) -shared $(CC_FLAGS) $^ -o $@ $(LD_FLAGS)
checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

