CC=g++
CC_FLAGS=-O2
ERROR_FILE=comp.errors

# Should detect current operating system (copied from stackoverflow)
ifeq ($(OS),Windows_NT)
	CC_FLAGS += -D WIN32
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		CC_FLAGS += -D AMD64
	else
		ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
			CC_FLAGS += -D AMD64
		endif
		ifeq ($(PROCESSOR_ARCHITECTURE),x86)
			CC_FLAGS += -D IA32
		endif
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CC_FLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CC_FLAGS += -D OSX
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CC_FLAGS += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CC_FLAGS += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CC_FLAGS += -D ARM
	endif
endif

ASM_DIR=asm/
PROC_DIR=proc/
LIB_DIR=lib/

OBJ_DIR=.obj/
DEP_DIR=.deps/

ASM_SRC=asm.cpp asm_main.cpp
PROC_SRC=proc.cpp proc-lib.cpp

all: $(OBJ_DIR) $(DEP_DIR) proc.out asm.out

-include $(addprefix $(DEP_DIR), *.d)

$(OBJ_DIR)%.o:*/%.cpp
	$(CC) -M $(CC_FLAGS) $< -o $(DEP_DIR)$(@F:.o=.d) 2> $(ERROR_FILE)
	echo "$(OBJ_DIR)" > line.tmp
	mv $(DEP_DIR)$(@F:.o=.d) input.tmp
	head -c -1 -q line.tmp input.tmp > $(DEP_DIR)$(@F:.o=.d)
	rm input.tmp line.tmp
	$(CC) -c $(CC_FLAGS) $< -o $@ 2> $(ERROR_FILE)

asm.out: $(addprefix $(OBJ_DIR),$(ASM_SRC:.cpp=.o) $(LIB_SRC:.cpp=.o))
	$(CC) $(CC_FLAGS) $^ -o $@ 2> $(ERROR_FILE)

proc.out: $(addprefix $(OBJ_DIR), $(PROC_SRC:.cpp=.o) $(LIB_SRC:.cpp=.o))
	$(CC) $(CC_FLAGS) $^ -o $@ 2> $(ERROR_FILE)

$(OBJ_DIR):
	mkdir $@

$(DEP_DIR):
	mkdir $@

clean:
	rm -rf $(DEP_DIR) $(OBJ_DIR)
	rm -rf $(ERROR_FILE) asm.out proc.out

.PHONY: clean