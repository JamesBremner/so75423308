#source file search paths
VPATH = ./src ../raven-set

	# compiler include search paths
INCS = -I./include \
	-I../ravencode/pub/src \
	-I../raven-set \
	-I../../code_ext/boost/boost1_78


LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
		-L../../code_ext/boost/boost1_78/lib \
		-lboost_system-mgw11-mt-x64-1_78 \

	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj
RDIR = ../ravencode/pub/src

OPT = -O2
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o \
	$(ODIR)/cRunWatch.o

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/starter.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

