CC = g++
CFLAGS = -Wall -Wno-unused-label -Wno-unused-variable -g
EXE = main
OBJS = main.o Token.o List.o Scanner.o Parser.o AST.o
TEST_DIR = ./test

ifdef OS # only windows define OS
	ifdef PATH # mingw
		RM = rm -f
	else
		RM = del
	endif
	EXE := $(EXE).exe
else
	RM = rm -f
endif

all: $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

$(OBJS): %.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(TEST_DIR)/*
	@for file in $^; do \
		echo $${file}; \
		./$(EXE) $${file}; \
		echo; \
	done

clean:
	$(RM) $(OBJS) $(EXE)
