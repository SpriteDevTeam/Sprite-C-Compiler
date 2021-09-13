CC = g++
CFLAGS = -Wall
EXE = main
OBJS = main.o Token.o List.o Scanner.o

ifdef OS
	RM = del
	EXE := $(EXE).exe
else
	RM = rm -f
endif

all: $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

$(OBJS): %.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

debug:
	./$(EXE) ./test/debug.sc

clean:
	$(RM) $(OBJS) $(EXE)
