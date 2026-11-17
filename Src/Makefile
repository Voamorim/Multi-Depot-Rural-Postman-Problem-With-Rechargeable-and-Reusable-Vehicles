COMPILER 		= g++

FLAGS   		= -std=c++17 -O3 -Wshadow -flto
LDFLAGS 		= -std=c++17 -O3 -flto -lm

BINARYDIR   	= Binary
EXENAME 		= main
MAIN_FILE 		= main
TARGET 			= $(BINARYDIR)/$(EXENAME)

MODULES_PATH 	= Modules

# Função que descobre recursivamente todos os modulos dentro da pasta Modules
rwildcard		= $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard, $d, $2) $(filter $(subst *, %, $2), $d))

# Busca todos os modulos
MODULE_DIRS		= $(sort $(dir $(call rwildcard, $(MODULES_PATH), *.hpp *.cpp)))

SRCS_LOCAL 		= $(filter-out $(MAIN_FILE).cpp, $(wildcard *.cpp))
SRCS_MODULES 	= $(call rwildcard, $(MODULES_PATH), *.cpp)

OBJS_MAIN		= $(BINARYDIR)/$(MAIN_FILE).o
OBJS_LOCAL		= $(patsubst %.cpp, $(BINARYDIR)/%.o, $(SRCS_LOCAL))
OBJS_MODULES 	= $(patsubst $(MODULES_PATH)/%.cpp, $(BINARYDIR)/modules/%.o, $(SRCS_MODULES))

OBJS 			= $(OBJS_LOCAL) $(OBJS_MODULES) $(OBJS_MAIN)
DEPS 			= $(OBJS:.o=.d)

INCLUDES 		= -I. $(addprefix -I, $(MODULE_DIRS))

.PHONY: all clean run

all: $(TARGET)

# Final binary
$(TARGET): $(OBJS) | $(BINARYDIR)
	$(COMPILER) $(OBJS) -o $@ $(LDFLAGS)

# Main file
$(BINARYDIR)/$(MAIN_FILE).o: $(MAIN_FILE).cpp | $(BINARYDIR)
	$(COMPILER) $(INCLUDES) $(FLAGS) -MMD -MP -c $< -o $@

# Local files
$(BINARYDIR)/%.o: %.cpp | $(BINARYDIR)
	@mkdir -p $(@D)
	$(COMPILER) $(INCLUDES) $(FLAGS) -MMD -MP -c $< -o $@

# Module files
$(BINARYDIR)/modules/%.o: $(MODULES_PATH)/%.cpp
	@mkdir -p $(@D)
	$(COMPILER) $(INCLUDES) $(FLAGS) -MMD -MP -c $< -o $@

$(BINARYDIR):
	mkdir -p $(BINARYDIR)

run: $(TARGET)
	./$(TARGET)	$(ARGS)

clean:
	rm -rf $(BINARYDIR)

-include $(DEPS)