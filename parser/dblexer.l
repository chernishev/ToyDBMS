%{
#include <iostream>
#include <math.h>
#include <string>

#include "dbscanner.h"
#undef YY_DECL
#define YY_DECL int ToyDBMS::DBScanner::yylex(ToyDBMS::DBParser::semantic_type * const lval, ToyDBMS::DBParser::location_type *loc)

using token = ToyDBMS::DBParser::token;

#define yyterminate() return(token::END)

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);


%}

%option DEBUG
%option nodefault
%option yyclass="ToyDBMS::DBScanner"
%option noyywrap
%option c++
%option caseless

ws       [ \t\n\r]+
DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*
STRING   ["][ a-zA-Z0-9!@#$%^&*()_+=.,/?-]*["]

%%
%{
    yylval = lval;
%}

{ws}

SELECT {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::SELECT);
}

DISTINCT {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::DISTINCT);
}

COUNT {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::COUNT);
}

MIN {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::MIN);
}

FROM {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::FROM);
}

AS {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::AS);
}

WHERE {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::WHERE);
}

GROUPBY {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::GROUPBY);
}

ORDERBY {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::ORDERBY);
}

AND {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::AND);
}

OR {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::OR);
}

IN {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::IN);
}

NOTIN {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::NOTIN);
}

{ID}+"."{ID}* {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    yylval->build<std::string> () = yytext;
    return (token::ATTRNAME);
}

{STRING}+ {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    auto remove_quotes = [](const char *str){
        std::string res(str);
        res.erase(0, 1);
        res.erase(res.size() - 1);
        return res;
    };
    yylval->build<std::string> () = remove_quotes(yytext);
    return (token::STR);
}

-?{DIGIT}+ {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    yylval->build<int> () = atoi(yytext);
    return (token::INT);
}

{ID} {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    yylval->build<std::string> () = yytext;
    return (token::TABLENAME);
}

[;|,|(|)|*] {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return yytext[0];
}

= {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::EQ);
}

\> {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::GR);
}

\< {
    #ifdef DBSCANDEBUG
        std::cerr<<yytext<<" ";
    #endif
    return (token::LE);
}


%%
