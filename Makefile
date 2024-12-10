SRC = src
BUILD = build
RELEASE = release
LIBDIR = lib
BINDIR = bin
INCLUDE = include
BIN = $(BINDIR)/pgne3k
LIB = $(LIBDIR)/pgne3k.a

PGN_LEX_H=$(INCLUDE)/pgn.lex.h
PGN_LEX_C=$(BUILD)/pgn.lex.c

PGN_SYNTAX_H=$(INCLUDE)/pgn.syntax.h
PGN_SYNTAX_C=$(BUILD)/pgn.syntax.c

SRCS=$(wildcard $(SRC)/*.c) 
OBJS=$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRCS) $(PGN_SYNTAX_C) $(PGN_LEX_C))
GENERATED_SRCS=

CFLAGS=-I$(SRC) -I$(INCLUDE) -Wall -Wextra -Wpedantic -g3
CC=clang

all: $(BIN)

$(BIN): $(PGN_SYNTAX_H) $(PGN_LEX_H) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PGN_LEX_H): $(BUILD)/pgn.lex.c

$(BUILD)/pgn.lex.c:
	flex --header-file=$(PGN_LEX_H) -o $@ $(SRC)/pgn.lex.l 

$(PGN_SYNTAX_H): $(BUILD)/pgn.syntax.c

$(BUILD)/pgn.syntax.c:
	bison $(SRC)/pgn.syntax.y --header=$(PGN_SYNTAX_H) -o $@

clean: $(INCLUDE) $(BUILD) $(BINDIR) $(LIBDIR)
	# For the sake of SAFETY, keep this logic simple and self contained.
	# I have rm -rf / SO many times.
	mkdir -p $(BUILD) $(BINDIR) $(LIBDIR)
	rm -fr $(BUILD)/* $(BINDIR)/* $(LIBDIR)/*
	rm -f $(OBJS)
	rm -f $(INCLUDE)/pgn.lex.h $(INCLUDE)/pgn.syntax.h

lint:
	find $(SRC) -name "*.c" -o -name "*.h" -exec clang-tidy $(CLANG_TIDY_ARGS) {} \;

compiledb:
	pip install compiledb

compile_commands.json: compiledb
	compiledb make

doc:
	doxygen Doxyfile

precheck:
	@make clean
	@make lint
	@make doc

fuzz: $(BIN)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-origins=yes  $(BIN) ./workspace/lichess/lichess-tag-roster.txt < data/test.txt

$(BINDIR):
	mkdir -p $@

$(LIBDIR):
	mkdir -p $@

$(INCLUDE):
	mkdir -p $@

$(OBJ):
	mkdir -p $@

$(BUILD):
	mkdir -p $@
