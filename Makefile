SRC = src
BUILD = build
RELEASE = release
LIBDIR = lib
BINDIR = bin
INCLUDE = include
LIB = $(LIBDIR)/pgne3k.a

PGN_LEX_H=$(INCLUDE)/pgn.lex.h
PGN_LEX_C=$(BUILD)/pgn.lex.c

PGN_SYNTAX_H=$(INCLUDE)/pgn.syntax.h
PGN_SYNTAX_C=$(BUILD)/pgn.syntax.c

SRCS=$(wildcard $(SRC)/*.c) $(PGN_SYNTAX_C) $(PGN_LEX_C)
OBJS=$(filter-out $(BUILD)/main.o, $(patsubst $(BUILD)/%.c,$(BUILD)/%.o, $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRCS))))

TEST=tests
TESTS=$(wildcard $(TEST)/*.c) 
TESTBINS=$(patsubst $(TEST)/%.c,$(BINDIR)/tests/%, $(TESTS))

PROJECT=projects
PROJECTS=$(wildcard $(PROJECT)/*.c) 
PROJECTBINS=$(patsubst $(PROJECT)/%.c,$(BINDIR)/%, $(PROJECTS))

TEST_TAGSPEC=projects/pgne3k/specs/quality-lichess.tagspec

CFLAGS=-I$(SRC) -I$(INCLUDE) -Wall -Wextra -Wpedantic -g3
CC=clang

all: $(TESTBINS) $(PROJECTBINS)

$(BUILD)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/tests/%: $(TEST)/%.c $(OBJS)
	mkdir -p $(BINDIR)/tests
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

$(BINDIR)/%: $(PROJECT)/%.c $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

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
	rm -f $(TESTBINS)
	rm -f $(PROJECTBINS)

lint:
	clang-tidy --quiet -header-filter=$(SRC)/.* $(SRC)/*.c $(SRC)/*.h -- $(CFLAGS) -fno-caret-diagnostics

compiledb:
	pip install compiledb

compile_commands.json: compiledb
	compiledb make

doc:
	doxygen Doxyfile

test: $(BIN) $(TEST)/bin $(TESTBINS) 
	for test in $(TESTBINS) ; do valgrind -q --leak-check=full --show-leak-kinds=all --track-origins=yes $$test ; done

precheck:
	@make clean
	@make lint
	@make doc

fuzz: $(PROJECTBINS)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BINDIR)/pgne3k $(TEST_TAGSPEC) < data/test.txt

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

$(TEST):
	mkdir -p $@

$(PROJECT):
	mkdir -p $@

$(BINDIR)/tests:
	mkdir -p $@


$(BINDIR)/projects:
	mkdir -p $@
