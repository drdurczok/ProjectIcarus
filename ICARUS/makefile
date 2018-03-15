CC = g++
TARGET = icarus
CFLAGS = -Wall
LINKER = g++ -o
OPENCV = `pkg-config opencv --cflags` `pkg-config opencv --libs`


SRCDIR   = src
HEADDIR  = include
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(HEADDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)


$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(OPENCV)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@ 
	@echo "Compiled "$<" successfully!"

clean : 
	rm -f bin/icarus obj/*.o 
	echo Clean done