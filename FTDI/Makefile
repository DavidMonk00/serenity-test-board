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
CFLAGS :=
CXXFLAGS := -std=c++11
FTDI_VERSION := $(shell find /usr/include/ -name "ftdi.h" | sed -e "s/\/usr\/include\///g")
INC := -Iinclude
ifeq ($(FTDI_VERSION), ftdi.h)
	LIBFTDI := ftdi
else
	LIBFTDI := ftdi1
	INC += -I/usr/include/libftdi1
endif
LIB := -l$(LIBFTDI)

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable: $(OBJECTS)
	@echo "Linking..."
	@echo "$(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(C_SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(CXX_SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning...";
	@echo "$(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)
	@echo "$(RM) include/*.gch"; $(RM) include/*.gch
