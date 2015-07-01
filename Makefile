# Directorys
BUILD = ./build
SRC = ./src
BIN = ./bin
PROGRAM_NAME = OcTr
DEBUGGER = gdb
ACCELERATOR = mpirun
PROCESSES = 2
ACCELERATOR_OPTIONS = -np $(PROCESSES)

# Build Flags and Libs
LIBS = -I/usr/include/boost \
	   -I/usr/include/boost/mpi \
	   -L/usr/lib \
	   -L/opt/bullxde/utils/boost/lib/\
	   -lboost_mpi \
	   -lboost_serialization

ADDCFLAGS = 
CFLAGS = -std=c++11 -Wall -g  $(ADDCFLAGS) #-O3 #-g for debugging
CC = mpic++

# Object and Shader definitions
_OBJECTS = $(shell find $(SRC)/* | grep .cpp)
OBJECTS = $(patsubst $(SRC)/%.cpp,$(BUILD)/%.o,$(_OBJECTS))
DEPENDENCIES = $(patsubst $(SRC)/%.cpp,$(BUILD)/%.P,$(_OBJECTS))

# Main Targets
$(BIN)/$(PROGRAM_NAME):  $(OBJECTS)
	@mkdir $(BIN) | true
	$(CC) -o $@ $(CFLAGS) $(LIBS) $(OBJECTS)

# Dependencies
$(sort $(DEPENDENCIES)): $(BUILD)/%.P: $(SRC)/%.cpp
	@mkdir $(BUILD) | true
	$(CC) -MM $(CFLAGS) -MT $(BUILD)/$*.o -MP -MF $@ $<

-include $(DEPENDENCIES)

# Object rules
$(sort $(OBJECTS)): $(BUILD)/%.o:
	$(CC) -c -o $@ $(CFLAGS) $(LIBS) $(SRC)/$*.cpp


# Utility Targets
.PHONY: run
run: $(BIN)/$(PROGRAM_NAME)
	$(ACCELERATOR) $(ACCELERATOR_OPTIONS) $<
	
.PHONY: debug
debug: $(BIN)/$(PROGRAM_NAME)
	$(ACCELERATOR) $(ACCELERATOR_OPTIONS) xterm -e $(DEBUGGER) $<

_FOLDER = $(shell tr -d ":" <<< `find src/* -type d`)
FOLDER = $(subst src, build, $(_FOLDER))
.PHONY: clean
clean:
	@echo "Cleaning Build Directory."
	@rm -rf $(BUILD)/*
	@rm -rf $(BIN)/*
	@echo "Creating empty directories."
	@mkdir $(FOLDER)
	@echo "Done."

.PHONY: directories
directories:
	@echo "Create all directories needed to build."
	@mkdir -f $(FOLDER)
	@echo "Done."
	
FILE_LIST = $(shell find src/*) Makefile
FILE_COUNT = $(shell ls -lR src | grep fabian | wc -l)
count:
	@wc -l $(FILE_LIST) || true
	@echo "Files:" $(FILE_COUNT)
