// Pure parser
%define api.pure full

/* Verbose error messages */
%define parse.error verbose

/* Enable location tracking */
%locations

%param { yyscan_t scanner }
%param { frontend_t *env }

%union {
  const char* str;
}

%token<str> WORD STRING RESULT MOVE_NUMBER CLOCK
%token OPEN_BRACKET CLOSE_BRACKET OPEN_CURLY CLOSE_CURLY QUOTE OPEN_PAREN CLOSE_PAREN


%code top {
  #include <stdio.h>
  #include <stdlib.h>
  #include <stddef.h>

  #include "game.h"
  #include "frontend.h"
  #include "board.h"
  #include "tag.h"
  #include "gameclock.h"
}

%code requires {
  #include "frontend.h"

  typedef void* yyscan_t;

  #define YY_DECL int yylex(YYSTYPE * yylval_param, YYLTYPE *yyloc, yyscan_t yyscanner, frontend_t *env)
}

%code {
  int yylex(YYSTYPE * yylval_param, YYLTYPE *yyloc, yyscan_t yyscanner, frontend_t *env);

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
}

tags
  :
  | tags tag
  ;

tag: OPEN_BRACKET WORD QUOTE STRING QUOTE CLOSE_BRACKET {
   taglist_add(env->games->tail->tags, $2, $4);
}

moves
  :
  | moves MOVE_NUMBER move {
    movelist_add(env->games->tail->moves, MOVE_TYPE_MOVE_NUMBER, $2);
  }
  | moves move
  ;

move: WORD extra {
    movelist_add(env->games->tail->moves, MOVE_TYPE_MOVE, $1);
    env->games->head->ply++;
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
      game_t *game = env->games->tail;
      if (game->ply % 2 == 0) {
        gameclock_add(game->clock_black, $2);
      } else {
        gameclock_add(game->clock_white, $2);
      }

      movelist_add(env->games->tail->moves, MOVE_TYPE_CLOCK, $2);
    }
  ;

variation: OPEN_PAREN STRING CLOSE_PAREN {
   movelist_add(env->games->tail->moves, MOVE_TYPE_VARIATION, $2);
}

%%
