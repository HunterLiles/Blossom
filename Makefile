# --- Configuration ---
# Compiler and Flags
CXX      = g++
CXXFLAGS = -std=c++20 -O0 -g -Wall
# Arch Linux Raylib Linker Flags (removes Windows specific flags like -lopengl32)
LDFLAGS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Files
SRC      = src/*.cpp
OUT_DIR  = bin
TARGET   = $(OUT_DIR)/Blossom

# --- Targets ---

# Default target (runs when you type 'make')
all: build

# 1. Compile the project
build:
	@mkdir -p $(OUT_DIR)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

# 2. Run the project (Equivalent to your run.ps1 logic)
run: build
	./$(TARGET)

# Clean up binaries
clean:
	rm -f $(TARGET)
