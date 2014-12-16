#####
# user configurable options
#####
CC := g++
INSTALL_DIR := bin
BUILD_PATH := build
PYTHON_VERSION := 2.7
PYTHON_PATH :=/usr/global/python/2.7.8/
BOOST_PATH := /usr/global/boost/1_55_0/
#####
# end configuration
#####


TARGET := libmdalyzer
PYTHON_INCLUDE := -I$(PYTHON_PATH)include/python$(PYTHON_VERSION)
BOOST_LIB := $(BOOST_PATH)lib/
CC_FLAGS := -fPIC -Wall -Wextra -pedantic
LD_FLAGS := -shared -Wl,-no-undefined,--export-dynamic -L$(BOOST_LIB) -lboost_python -lpython$(PYTHON_VERSION)

MODULES := analyzers extern data_structures python trajectories utils
SRC_DIR := $(addprefix $(TARGET)/,$(MODULES))
BUILD_DIR := $(addprefix $(BUILD_PATH)/,$(MODULES))

SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
OBJ := $(patsubst $(TARGET)/%.cc,build/%.o,$(SRC))
INCLUDES := $(addprefix -I,$(SRC_DIR)) $(PYTHON_INCLUDE)

vpath %.cc $(SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CC) $(INCLUDES) $(CC_FLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean install

all: checkdirs $(BUILD_PATH)/$(TARGET).so
build/$(TARGET).so: $(OBJ)
	$(CC) $(LD_FLAGS) $^ -o $@
checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_PATH)/*
	@rm -rf $(INSTALL_DIR)/mdalyzer/*.pyc

install:
	@mkdir -p $(INSTALL_DIR)
	@cp $(BUILD_PATH)/$(TARGET).so $(INSTALL_DIR)/
	@cp -r mdalyzer $(INSTALL_DIR)/

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

