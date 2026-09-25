# Makefile for GPU-Accelerated Canny Edge Detection using NVIDIA NPP

CUDA_PATH ?= /usr/local/cuda
NVCC      ?= $(CUDA_PATH)/bin/nvcc
CXX       ?= g++

CXXFLAGS  := -O3 -std=c++14 -Wall -Wextra -Iinclude -Ithird_party -I$(CUDA_PATH)/include
NVCCFLAGS := -O3 -std=c++14 -Iinclude -Ithird_party -I$(CUDA_PATH)/include

# NPP libraries subset required for filtering (nppiFilterCannyBorder) and core:
#   -lnppif : NPP Image Filtering library
#   -lnppc  : NPP Core library
#   -lcudart: CUDA Runtime library
NPP_LIBS  := -lnppif -lnppc -lcudart -L$(CUDA_PATH)/lib64

BIN_DIR   := bin
SRC_DIR   := src
OBJ_DIR   := obj

TARGET    := $(BIN_DIR)/canny_npp

CPP_SRCS  := $(SRC_DIR)/main.cpp $(SRC_DIR)/image_io.cpp
CU_SRCS   := $(SRC_DIR)/canny_npp.cu

CPP_OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SRCS))
CU_OBJS   := $(patsubst $(SRC_DIR)/%.cu,$(OBJ_DIR)/%.o,$(CU_SRCS))
OBJS      := $(CPP_OBJS) $(CU_OBJS)

.PHONY: all build run clean check_cuda

all: build

check_cuda:
	@which $(NVCC) > /dev/null 2>&1 || (echo "Error: nvcc compiler not found at $(NVCC). Please install CUDA Toolkit." && exit 1)
	@test -f $(CUDA_PATH)/include/nppi.h || (echo "Error: nppi.h not found in $(CUDA_PATH)/include. Please install NVIDIA NPP." && exit 1)

build: check_cuda $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking executable $(TARGET)..."
	$(NVCC) $(OBJS) -o $(TARGET) $(NPP_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling C++ source $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu | $(OBJ_DIR)
	@echo "Compiling CUDA source $<..."
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: build
	@echo "Running canny_npp --help..."
	./$(TARGET) --help

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
