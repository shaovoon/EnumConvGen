# Project: ParseEnum

#CPP      = clang++ 
CPP      = g++-4.7  
CC       = gcc
OBJ      = ParseEnum.o EnumStrConv.o $(RES)
LINKOBJ  = ParseEnum.o EnumStrConv.o $(RES)
LIBS     = 
CXXINCS  = 
BIN      = ParseEnum
CXXFLAGS = -Wall -std=c++11 -g -O1 $(CXXINCS)
CFLAGS   = -Wall -std=c++11 -g -O1  
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ParseEnum all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "ParseEnum" $(LIBS)

ParseEnum.o: ParseEnum.cpp
	$(CPP) -c ParseEnum.cpp -o ParseEnum.o $(CXXFLAGS)

EnumStrConv.o: EnumStrConv.cpp
	$(CPP) -c EnumStrConv.cpp -o EnumStrConv.o $(CXXFLAGS)
