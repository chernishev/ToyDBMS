#pragma once

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "dbparser.tab.hh"
#include "location.hh"

namespace ToyDBMS{

    class DBScanner : public yyFlexLexer {
    public:
        DBScanner(std::istream *in) : yyFlexLexer(in){};
        virtual ~DBScanner(){};
        using FlexLexer::yylex;
        virtual int yylex(ToyDBMS::DBParser::semantic_type * const lval,
                                      ToyDBMS::DBParser::location_type *location);
    private:
        ToyDBMS::DBParser::semantic_type *yylval = nullptr;
    };

}
