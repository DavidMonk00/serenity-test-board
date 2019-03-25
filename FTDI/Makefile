CC := gcc
CXX := g++
SRCDIR := src
BUILDDIR := build
TARGET := bin/main

C_SRCEXT := c
CXX_SRCEXT := cpp
C_SOURCES := $(shell find $(SRCDIR) -type f -name *.$(C_SRCEXT))
CXX_SOURCES := $(shell find $(SRCDIR) -type f -name *.$(CXX_SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(C_SOURCES:.$(C_SRCEXT)=.o))
OBJECTS += $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(CXX_SOURCES:.$(CXX_SRCEXT)=.o))
CFLAGS := -g
CXXFLAGS := -g
LIB := -lmpsse -lm
INC := -Iinclude

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@echo "$(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(C_SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(CXX_SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning...";
	@echo "$(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)
