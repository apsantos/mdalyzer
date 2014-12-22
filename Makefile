#####
# user configurable options
#####
CC := gcc
CXX := g++
INSTALL_PATH := bin
BUILD_PATH := build
PYTHON_VERSION := 2.7
PYTHON_PATH :=/usr/global/python/2.7.8/
BOOST_PATH := /usr/global/boost/1_55_0/
#####
# end configuration
#####


TARGET := libmdalyzer
PYTHON_INCLUDE := -I$(PYTHON_PATH)/include/python$(PYTHON_VERSION)
BOOST_LIB := $(BOOST_PATH)/lib/
CXXFLAGS := -fPIC -Wall -Wextra -pedantic
CFLAGS := $(CXXFLAGS) --std=c99
LDFLAGS := -shared -Wl,-no-undefined,--export-dynamic -L$(BOOST_LIB) -lboost_python -lpython$(PYTHON_VERSION)

MODULES := analyzers extern data_structures python trajectories utils
SRC_DIR := $(addprefix $(TARGET)/,$(MODULES))
BUILD_DIR := $(addprefix $(BUILD_PATH)/,$(MODULES))

SRC_CXX := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
SRC_CC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ := $(patsubst $(TARGET)/%.cc,$(BUILD_PATH)/%.o,$(SRC_CXX)) $(patsubst $(TARGET)/%.c, $(BUILD_PATH)/%.o,$(SRC_CC))
INCLUDES := $(addprefix -I,$(SRC_DIR)) $(PYTHON_INCLUDE)

vpath %.cc $(SRC_DIR)
vpath %.c $(SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $$< -o $$@
$1/%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean install

all: checkdirs $(BUILD_PATH)/$(TARGET).so
$(BUILD_PATH)/$(TARGET).so: $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@
checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_PATH)/*
	@rm -rf $(INSTALL_PATH)/mdalyzer/*.pyc

install:
	@mkdir -p $(INSTALL_PATH)
	@cp $(BUILD_PATH)/$(TARGET).so $(INSTALL_PATH)/
	@cp -r mdalyzer $(INSTALL_PATH)/

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

