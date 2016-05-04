CC=g++ -m64
CFLAGS=-std=c++11
CDEBUGFLAGS=-std=c++11 -O0 -g -DDEBUG
LDFLAGS=
EXECUTABLE=encod
# source cc files with headers:
HEADED=code.cc utf8.cc
# source cc files without headers:
NON_HEADED=main.cc
# also add any other remaining headers to this guy:
REMAINING_HEADERS=

# no need to modify past this point
HEADERS=$(HEADED:.cc=.h) $(REMAINING_HEADERS)
SYS := $(shell $(CC) -dumpmachine)
OBJECTS=$(NON_HEADED:%.cc=$(SYS)/%.o) $(HEADED:%.cc=$(SYS)/%.o)
#OBJECTS=$(NON_HEADED:%.cc=%.$(SYS).o) $(HEADED:%.cc=%.$(SYS).o)

.PHONY: build, clean, debug, clean-o, clean-g

build: $(SYS) $(EXECUTABLE).$(SYS)

$(SYS):
	mkdir -p $(SYS)

debug:  clean
	$(MAKE) M=`pwd` CFLAGS="$(CDEBUGFLAGS)" EXECUTABLE="$(EXECUTABLE)-g"
	$(MAKE) M=`pwd` clean-o

$(EXECUTABLE).$(SYS): $(OBJECTS) clean-g
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ 

$(SYS)/%.o: %.cc $(HEADERS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean-o:
	rm -f $(SYS)/*.o 

clean-g:
	rm -f $(EXECUTABLE)-g.$(SYS)

clean: clean-o clean-g
	rm -f $(EXECUTABLE).$(SYS)
 
# DO NOT DELETE

