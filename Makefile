#####
# user configurable options
#####
CC := gcc
CXX := g++
INSTALL_PATH := bin
BUILD_PATH := build
PYTHON_VERSION := 2.7
PYTHON_INCLUDE_PATH := /usr/global/python/2.7.8/include/python2.7/
BOOST_PATH := /usr/global/boost/1_55_0/
#####
# end configuration
#####

### for compiling shared library libmdalyzer ###
TARGET := libmdalyzer
CXXFLAGS := -fPIC -Wall -Wextra -pedantic -O3
CFLAGS := $(CXXFLAGS) --std=c99
LDFLAGS := -shared -Wl,-no-undefined,--export-dynamic -Wl,-soname,$(TARGET).so -L$(BOOST_PATH)/lib -L$(BOOST_PATH)/lib64 -lboost_python -lpython$(PYTHON_VERSION)

MODULES := analyzers extern data_structures python trajectories utils
SRC_DIR := $(addprefix $(TARGET)/,$(MODULES))
BUILD_DIR := $(addprefix $(BUILD_PATH)/$(TARGET)/,$(MODULES)) $(BUILD_PATH)/test/unit

SRC_CXX := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
SRC_CC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ := $(patsubst $(TARGET)/%.cc,$(BUILD_PATH)/$(TARGET)/%.o,$(SRC_CXX)) $(patsubst $(TARGET)/%.c, $(BUILD_PATH)/$(TARGET)/%.o,$(SRC_CC))
INCLUDES := $(addprefix -I,$(SRC_DIR)) -I$(PYTHON_INCLUDE_PATH) -I$(BOOST_PATH)/include
### end shared library ###

### for compiling boost unit tests ###
TEST_TARGET := test_libmdalyzer
TEST_DIR := test
TEST_SRC_DIR := $(TEST_DIR)/unit
TEST_SRC := $(wildcard $(TEST_SRC_DIR)/*.cc)
TEST_OBJ := $(patsubst $(TEST_SRC_DIR)/%.cc,$(BUILD_PATH)/$(TEST_SRC_DIR)/%.o,$(TEST_SRC))
TEST_CXXFLAGS := -Wall -Wextra -pedantic -O3
TEST_LDFLAGS := -Wl,-rpath,$(dir $(abspath $(BUILD_PATH)/$(TARGET).so)) -L$(BUILD_PATH) -lmdalyzer

TEST_PYTHON_TARGET := test_mdalyzer.py
TEST_PYTHON_MODULE := $(TEST_DIR)/mdalyzer
### end unit test ###

# set the path for cc and c files
SEARCH_DIR := $(SRC_DIR) $(TEST_SRC_DIR)
vpath %.cc $(SEARCH_DIR)
vpath %.c $(SEARCH_DIR)

# will be called to define make rules all subdirectories
define make-goal
$1/%.o: %.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $$< -o $$@
$1/%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean install check doc py

all: checkdirs $(BUILD_PATH)/$(TARGET).so
	@cp -r mdalyzer $(BUILD_PATH)

$(BUILD_PATH)/$(TARGET).so: $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

# check compiles and runs the boost and python unit tests
check: all $(BUILD_PATH)/$(TEST_TARGET)
	@cp -r $(TEST_PYTHON_MODULE) $(BUILD_PATH)/$(TEST_DIR)
	@cp $(TEST_DIR)/$(TEST_PYTHON_TARGET) $(BUILD_PATH)
	python $(BUILD_PATH)/$(TEST_PYTHON_TARGET) $(BUILD_PATH) $(TEST_DIR)

	@echo ""
	@echo ""
	@$(BUILD_PATH)/$(TEST_TARGET) --log_level=message

$(BUILD_PATH)/$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(TEST_LDFLAGS) $^ -o $@

# recursively builds subdirectories as needed
checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

# cleanup the build objects and the python byte code
clean:
	@rm -rf $(BUILD_PATH)/*
	@rm -rf $(INSTALL_PATH)/mdalyzer/*.pyc

install: all
	@mkdir -p $(INSTALL_PATH)
	@cp $(BUILD_PATH)/$(TARGET).so $(INSTALL_PATH)/
	@cp -r mdalyzer $(INSTALL_PATH)/

doc:
	@doxygen doc/Doxyfile_user.in

# iteratively define build rules for subdirectories
$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
