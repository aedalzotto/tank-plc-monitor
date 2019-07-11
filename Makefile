BIN := tank-plc-monitor

CXX = g++
CP  := cp
MK  := mkdir
RM  := rm
PKGCFG := pkg-config

SRCDIR   := src
INCDIR 	 := $(SRCDIR)/include
ASSETDIR := assets

BUILDDIR := build
TMPDIR 	 := tmp

BINDIR 	 := $(BUILDDIR)/bin
SHAREDIR := $(BUILDDIR)/share/$(BIN)

SRCEXT   := cpp

TARGET := $(BINDIR)/$(BIN)

SRCEXT 	:= cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(addprefix $(TMPDIR)/,$(notdir $(SOURCES:.$(SRCEXT)=.o)))
ASSETS  := $(wildcard $(ASSETDIR)/*.png) $(wildcard $(ASSETDIR)/*.glade)
SHAREFILES := $(addprefix $(SHAREDIR)/,$(notdir $(ASSETS)))

TARGET 	 := $(BINDIR)/$(BIN)
CXXFLAGS := -g -Wall -std=c++17 `pkg-config --cflags gtkmm-3.0 glibmm-2.4`
LDFLAGS  := `pkg-config --libs gtkmm-3.0` -lnodave -lstdc++fs -pthread
INCLUDES := -I./$(INCDIR)

all: $(TARGET) $(SHAREFILES)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(MK) -p $(BINDIR)
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)

$(TMPDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling $<..."
	@$(MK) -p $(TMPDIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

$(SHAREDIR)/%: $(ASSETDIR)/%
	@echo "Copying $<..."
	@$(MK) -p $(SHAREDIR)
	@$(CP) $< $@

clean:
	@echo "Cleaning..."
	@$(RM) -rf $(BUILDDIR) $(TMPDIR)

.PHONY: clean