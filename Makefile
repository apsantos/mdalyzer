CC := g++

INSTALL_DIR := ~/apc_524/mdalyzer/tmp
TARGET := mdalyzer
CC_FLAGS := -fPIC -Wall -Wextra -pedantic
LD_FLAGS := -shared -Wl,-no-undefined,--export-dynamic -L/usr/global/boost/1_55_0/lib/ -lboost_python -lpython2.7
PYTHON_INCLUDE := -I/usr/include/python2.7

MODULES := computes extern frames python trajectory utils
SRC_DIR := $(addprefix lib$(TARGET)/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
OBJ := $(patsubst lib$(TARGET)/%.cc,build/%.o,$(SRC))
INCLUDES := $(addprefix -I,$(SRC_DIR)) $(PYTHON_INCLUDE)

vpath %.cc $(SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CC) $(INCLUDES) $(CC_FLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean install

all: checkdirs build/$(TARGET).so
build/$(TARGET).so: $(OBJ)
	$(CC) $(LD_FLAGS) $^ -o $@
checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

install:
	@mkdir -p $(INSTALL_DIR)
	@cp build/$(TARGET).so $(INSTALL_DIR)/

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

