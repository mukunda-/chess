SRC = src
BUILD = build
EXECUTABLE = $(BUILD)/bin/pgne3k

SRCS=$(wildcard $(SRC)/*.c)

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)/pgn.l $(SRC)/pgn.y $(SRCS)
	mkdir -p $(BUILD)/bin
	bison -d $(SRC)/pgn.y -o $(BUILD)/pgn.tab.c --header=$(BUILD)/pgn.tab.h
	flex --header-file=$(BUILD)/pgn.lex.h -o $(BUILD)/pgn.yy.c $(SRC)/pgn.l 
	cc -g -I $(BUILD) -I $(SRC) -o $@ $(BUILD)/*.c $(SRCS) -lfl

clean:
	rm -fr $(BUILD)/*

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
test: $(EXECUTABLE)
	$(EXECUTABLE) ./lichess-tag-roster.txt < data/test.txt
