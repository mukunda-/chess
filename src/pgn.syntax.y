// Pure parser
%define api.pure full

/* Verbose error messages */
%define parse.error verbose

/* Enable location tracking */
%locations

%param { yyscan_t scanner }
%param { pgn_frontend_t *env }

%union {
  const char* str;
}

%token<str> WORD STRING RESULT MOVE_NUMBER CLOCK
%token OPEN_BRACKET CLOSE_BRACKET OPEN_CURLY CLOSE_CURLY QUOTE OPEN_PAREN CLOSE_PAREN


%code top {
  #include <stdio.h>
  #include <stdlib.h>
  #include <stddef.h>

  #include "pgn.h"
  #include "pgn_frontend.h"
  #include "board.h"
  #include "pgn_tag.h"
  #include "gameclock.h"
}

%code requires {
  #include "pgn_frontend.h"

  typedef void* yyscan_t;

  #define YY_DECL int yylex(YYSTYPE * yylval_param, YYLTYPE *yyloc, yyscan_t yyscanner, pgn_frontend_t *env)
}

%code {
  int yylex(YYSTYPE * yylval_param, YYLTYPE *yyloc, yyscan_t yyscanner, pgn_frontend_t *env);

  void yyerror(YYLTYPE *yyloc, yyscan_t unused, pgn_frontend_t *env,
             const char *msg);
  void flush_pgns(pgn_frontend_t *env);
}

%%

pgns
  :
  | pgns pgn 
  ;

pgn: tags moves RESULT {
    pgnlist_add_result(env->pgns, $3);

    flush_pgns(env);
}

tags
  :
  | tags tag
  ;

tag: OPEN_BRACKET WORD QUOTE STRING QUOTE CLOSE_BRACKET {
   taglist_add(env->pgns->tail->tags, $2, $4);
}

moves
  :
  | moves MOVE_NUMBER move {
    pgn_movelist_add(env->pgns->tail->moves, PGN_MOVETYPE_MOVE_NUMBER, $2);
  }
  | moves move
  ;

move: WORD extra {
    pgn_movelist_add(env->pgns->tail->moves, PGN_MOVETYPE_MOVE, $1);
    env->pgns->head->ply++;
}

extra
 :
 | extra comment
 | extra variation
 ;

comment: OPEN_CURLY comment_parts CLOSE_CURLY
       ;

comment_parts
  :
  | comment_parts CLOCK {
      pgn_t *pgn = env->pgns->tail;
      if (pgn->ply % 2 == 0) {
        gameclock_add(pgn->clock_black, $2);
      } else {
        gameclock_add(pgn->clock_white, $2);
      }

      pgn_movelist_add(env->pgns->tail->moves, PGN_MOVETYPE_CLOCK, $2);
    }
  ;

variation: OPEN_PAREN STRING CLOSE_PAREN {
   pgn_movelist_add(env->pgns->tail->moves, PGN_MOVETYPE_VARIATION, $2);
}

%%
