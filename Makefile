SRC = src
BUILD = build
RELEASE = release
LIBDIR = lib
BINDIR = bin
INCLUDE = include
BIN = $(BINDIR)/pgne3k
LIB = $(LIBDIR)/pgne3k.a


SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(pathsub $(SRC)/*.c, $(OBJ)/%.o, $(SRCS))

all: $(BIN)

$(BIN): $(SRC)/pgn.lex.l $(SRC)/pgn.syntax.y $(SRCS) $(INCLUDE) $(BUILD) $(BINDIR)
	bison -d $(SRC)/pgn.syntax.y -o $(BUILD)/pgn.syntax.c --header=$(INCLUDE)/pgn.syntax.h
	flex --header-file=$(INCLUDE)/pgn.lex.h -o $(BUILD)/pgn.lex.c $(SRC)/pgn.lex.l 
	cc -g3 -I $(INCLUDE) -I $(SRC) -o $@ $(BUILD)/*.c $(SRCS)

clean: $(INCLUDE) $(BUILD) $(BINDIR) $(LIBDIR)
	# For the sake of SAFETY, keep this logic simple and self contained.
	# I have rm -rf / SO many times.
	mkdir -p $(BUILD) $(BINDIR) $(LIBDIR)
	rm -r    $(BUILD) $(BINDIR) $(LIBDIR)
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
	mkdir -p $(BINDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(INCLUDE):
	mkdir -p $(INCLUDE)

$(OBJ):
	mkdir -p $(OBJ)

$(BUILD):
	mkdir -p $(BUILD)
