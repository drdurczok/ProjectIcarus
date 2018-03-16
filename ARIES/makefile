CC = g++ -std=c++0x
TARGET = aries
LINKER = g++ -o
SPHINX = -I/usr/include/speech_tools -lasound -lFestival -lestools -lestbase -leststring  `pkg-config --cflags --libs pocketsphinx sphinxbase` -DMODELDIR=\"`pkg-config --variable=modeldir pocketsphinx`\" 


SRCDIR   = src
HEADDIR  = include
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(HEADDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)


$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(SPHINX)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@ $(SPHINX)
	@echo "Compiled "$<" successfully!"

clean : 
	rm -f bin/icarus obj/*.o 
	echo Clean done

