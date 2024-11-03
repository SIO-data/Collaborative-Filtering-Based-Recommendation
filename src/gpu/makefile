# Compiler
CXX = icpx

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11 -fsycl

# Linker flags
LDFLAGS = -L"C:\Program Files (x86)\Intel\oneAPI\compiler\2025.0\lib" -lsycl8

# Source files
SRC = main.cpp mycsv.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable name
TARGET = recommendation_system.exe

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean
