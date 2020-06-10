
LIBSCVER=1.8

# Files
SOURCES=slidecontroller.cpp
SOURCES+=scal.cpp
LIBSC=../../../common/$(LIBSCVER)/lib/libsc.a
EXECUTABLE=slidecontroller
OBJECTS=$(SOURCES:.cpp=.o)

# Dirs
LIBSCDIR=../../../common/$(LIBSCVER)/lib/
INCLUDEDIR=/usr/include
LIBDIR=/usr/lib

# Tools
CC=$(CROSS_COMPILE)g++
LD=ld

# Flags
PRJCFLAGS=-g -c -Wall -I$(INCLUDEDIR) -I$(LIBSCDIR) -std=c++0x #-E
PRJLDFLAGS=-I$(LIBDIR) -L$(LIBSCDIR)
CFLAGS=$(PRJCFLAGS) 
# ??? -pthread
# -Wno-write-strings
LDFLAGS=$(PRJLDFLAGS) -lcurl -lmpdclient -lmysqlclient -lsc -lpthread


# Targets
all: $(SOURCES) $(EXECUTABLE)
	@echo "This is -all-"

$(EXECUTABLE): $(OBJECTS) $(LIB)
	@echo "This is -$(EXECUTABLE)-"
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBSC) -o $@
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	@echo "This is -.cpp.o- for $<"
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo "This is -clean-"
	rm -f $(EXECUTABLE) $(OBJECTS)