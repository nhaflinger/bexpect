%option noyywrap 
%{

  #include "blang.tab.hh"

  int line_num = 1;

%}
%%
[ \t] ;
start                 { return START; }
end                   { return END; }
wait                  { return WAIT; }
send                  { return SEND; }
expect                { return EXPECT; }
[0-9]+\.[0-9]+        { yylval.fval = atof(yytext); return FLOAT; }
[0-9]+                { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+          { yylval.sval = strdup(yytext); return STRING; }
(x\"[a-zA-Z0-9 :]+\"[ \t]*)+    { yylval.sval = strdup(yytext); return HEX_QUOTES; }
\"[a-zA-Z0-9 ]+\"     { yylval.sval = strdup(yytext); return STRING_QUOTES; }
#[^\n]+               { return COMMENT; }
;                     { return ENDL; }
\n                    { ++line_num; }
%%

