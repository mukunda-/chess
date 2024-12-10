// Pure parser
%define api.pure full

/* Verbose error messages */
%define parse.error verbose

/* Enable location tracking */
%locations

%param { yyscan_t scanner }
%parse-param { frontend_t *env }

%union {
  char* str;
}

%token<str> WORD STRING RESULT MOVE_NUMBER 
%token OPEN_BRACKET CLOSE_BRACKET OPEN_CURLY CLOSE_CURLY QUOTE OPEN_PAREN CLOSE_PAREN


%code top {
  #include <stdio.h>
  #include <stdlib.h>
  #include <stddef.h>

  #include "gamelist.h"
  #include "frontend.h"
  #include "taglist.h"
}

%code requires {
  #include "frontend.h"

  typedef void* yyscan_t;

  #define YY_DECL int yylex(YYSTYPE * yylval_param, YYLTYPE *yyloc, yyscan_t yyscanner)
}

%code {
  int yylex(YYSTYPE * yylval_param, YYLTYPE *yyloc, yyscan_t yyscanner);

  void yyerror(YYLTYPE *yyloc, yyscan_t unused, frontend_t *env,
             const char *msg);
  void flush_games(frontend_t *env);
}

%%

pgns
  :
  | pgns pgn 
  ;

pgn: tags moves RESULT {
    gamelist_add_result(env->games, $3);

    flush_games(env);

    free($3);
}

tags
  :
  | tags tag
  ;

tag: OPEN_BRACKET WORD QUOTE STRING QUOTE CLOSE_BRACKET {
   taglist_add(env->games->tail->tags, $2, $4);

   free($2);
   free($4);
}

moves
  :
  | moves MOVE_NUMBER move {
    movelist_add(env->games->tail->moves, MOVE_TYPE_MOVE_NUMBER, $2);

    free($2);
  }
  | moves move
  ;

move: WORD extra {
    movelist_add(env->games->tail->moves, MOVE_TYPE_MOVE, $1);
    env->games->head->ply++;
    free($1);
}

extra
 :
 | extra comment
 | extra variation
 ;

comment: OPEN_CURLY STRING CLOSE_CURLY {
    movelist_add(env->games->tail->moves, MOVE_TYPE_COMMENT, $2);
    free($2);
}

variation: OPEN_PAREN STRING CLOSE_PAREN {
   movelist_add(env->games->tail->moves, MOVE_TYPE_VARIATION, $2);
   free($2);
}

%%
