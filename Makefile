# rigid_body_2d Makefile

# vars
CC     = g++
CFLAGS = -std=c++11 -Wall -Wextra -Werror
LINK   = -lSDL2 -lGL
EXE    = rigid_quads

# compiled objects
OBJS  = rigid_quad_2d.o
OBJS += main.o

all: release

# targets
release: CFLAGS += -O3 -DNDEBUG
release: clean_exe type_release_build full_build

debug: CFLAGS += -g
debug: clean_exe type_debug_build full_build

clean: clean_objs clean_exe

# helper targets
type_release_build:
	$(info *************************** Preforming Release Build ****************************)

type_debug_build:
	$(info **************************** Preforming Debug Build *****************************)

full_build: build_objs $(OBJS) link_build $(EXE)
	$(info )
	$(info ******************************* Successful Build ********************************)
	$(info Executable located at: $(EXE))
	$(info Run from inside the bin directory for now)
	$(info )

build_objs:
	$(info )
	$(info ******************************* Building Objects ********************************)

link_build:
	$(info )
	$(info ********************************* Linking Exe ***********************************)

clean_objs:
	$(info )
	$(info ****************************** Cleaning Objects *********************************)
	rm -f $(OBJS)

clean_exe:
	$(info )
	$(info ******************************** Cleaning Exe ***********************************)
	rm -f $(EXE)	

# invidual compiler invocations
%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(EXE): main.o
	$(CC) $(CFLAGS) $(LINK) -o $@ $(OBJS)
 
