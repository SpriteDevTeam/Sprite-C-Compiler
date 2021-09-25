CC = g++
CFLAGS = -Wall -Wno-unused-label -Wno-unused-variable -g
EXE = main
OBJS = main.o Token.o List.o Scanner.o Parser.o CST.o
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

ifndef MARK
	MARK = '<expr>\|$$'
endif

all: $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

$(OBJS): %.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(EXE) ./test/$(FILE) | grep --color $(MARK)

debug: $(TEST_DIR)/*
	@for file in $^; do \
		echo $${file}; \
		./$(EXE) $${file} | grep --color $(MARK); \
		echo; \
	done

clean:
	$(RM) $(OBJS) $(EXE)
