%option 8bit reentrant bison-bridge
%option warn noyywrap nodefault

%top {
#include <stdlib.h>

#include "pgn.syntax.h"

#define RETURN_TOKEN_NEXT(token, next_state) { \
               BEGIN(next_state); \
               return token; \
             } \

#define RETURN_TOKEN(token) return token;

}

%x TOKENIZE_TAG_NAME TOKENIZE_STRING
%x TOKENIZE_COMMENT TOKENIZE_VARIATION

%%

<INITIAL>{
  [[:space:]]   ;

  \[ RETURN_TOKEN(OPEN_BRACKET)

  \] RETURN_TOKEN(CLOSE_BRACKET)

  \{ RETURN_TOKEN_NEXT(OPEN_CURLY, TOKENIZE_COMMENT)

  \( RETURN_TOKEN_NEXT(OPEN_PAREN, TOKENIZE_VARIATION)

  \" RETURN_TOKEN_NEXT(QUOTE, TOKENIZE_STRING)

  "1-0"|"0-1"|"1/2-1/2"|"*" {
     yylval->str = strdup(yytext);

     return RESULT;
   }

  [1-9][0-9]*(\.\.\.|\.) {
    yylval->str = strdup(yytext);

    return MOVE_NUMBER;
  }

  [a-zA-Z0-9\.+#!?\-=]+ {
    yylval->str = strdup(yytext);

    return WORD;
  }
}

<TOKENIZE_STRING>{
  [^"]+ {
    yylval->str = strdup(yytext);

    return STRING;
  }
  \" RETURN_TOKEN_NEXT(QUOTE, INITIAL)
}

<TOKENIZE_VARIATION>{
  [^)]* {
    yylval->str = strdup(yytext);

    return STRING;
  }
  ")" RETURN_TOKEN_NEXT(CLOSE_PAREN, INITIAL);
}


<TOKENIZE_COMMENT>{
  [^}]* {
    yylval->str = strdup(yytext);

    return STRING;
  }
  "}" RETURN_TOKEN_NEXT(CLOSE_CURLY, INITIAL)
}

%%
