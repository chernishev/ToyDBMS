%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%define api.namespace {ToyDBMS}
%define parser_class_name {DBParser}

%code requires{
    namespace ToyDBMS {
        class DBScanner;
    }

    #include "query.h"

// The following definitions are missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif
}



%parse-param { DBScanner  &scanner } { Query &result_query }

%code{
    #include <iterator>
    #include "dbscanner.h"

    #undef yylex
    #define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token END    0     "end of file"

%token <std::string> ATTRNAME  "fully-qualified attribute name"
%token <std::string> TABLENAME "table name"
%token <int>         INT       "integer value"
%token <std::string> STR       "string value"

%token SELECT
%token DISTINCT
%token COUNT
%token MIN
%token FROM
%token AS
%token WHERE
%token GROUPBY
%token ORDERBY
%token AND
%token OR
%token IN
%token NOTIN

%token GR
%token EQ
%token LE

%left AND OR

%type <bool> distinct
%type <std::unique_ptr<Query>> query basicquery
%type <std::unique_ptr<Predicate>> whparams predicate
%type <std::vector<std::string>> attrlist
%type <Predicate::Relation> relation
%type <SelectionPart> selattr
%type <std::vector<SelectionPart>> selattrs
%type <SelectionClause> selparams
%type <std::unique_ptr<FromPart>> frparam
%type <std::vector<std::unique_ptr<FromPart>>> frparams

%locations

%%

whole_query: query { result_query = std::move(*$1); }

query:
    basicquery WHERE whparams GROUPBY attrlist ORDERBY attrlist ';'
    { $1->where = std::move($3); $1->groupby = std::move($5); $1->orderby = std::move($7); $$ = std::move($1); }
    | basicquery WHERE whparams ORDERBY attrlist ';'
    { $1->where = std::move($3); $1->orderby = std::move($5); $$ = std::move($1); }
    | basicquery WHERE whparams GROUPBY attrlist ';'
    { $1->where = std::move($3); $1->groupby = std::move($5); $$ = std::move($1); }
    | basicquery GROUPBY attrlist ORDERBY attrlist ';'
    { $1->groupby = std::move($3); $$ = std::move($1); }
    | basicquery GROUPBY attrlist ';'
    { $1->groupby = std::move($3); $$ = std::move($1); }
    | basicquery ORDERBY attrlist ';'
    { $1->orderby = std::move($3); $$ = std::move($1); }
    | basicquery WHERE whparams ';'
    { $1->where = std::move($3); $$ = std::move($1); }
    | basicquery ';' { $$ = std::move($1); }
    ;

basicquery:
    SELECT distinct selparams FROM frparams {
        $$ = std::make_unique<Query>();
        $$->distinct = $2;
        $$->selection = std::move($3);
        $$->from = std::move($5);
    };

distinct:
    %empty  { $$ = false; }
    | DISTINCT { $$ = true;}
    ;


selparams:
    selattrs { $$.type = SelectionClause::Type::LIST; $$.attrs = std::move($1); }
    | COUNT '(' '*' ')' { $$.type = SelectionClause::Type::COUNT; }
    | '*' { $$.type = SelectionClause::Type::ALL; }
    ;

selattrs:
    selattr { $$.push_back(std::move($1)); }
    | selattr ',' selattrs {
        $$.push_back(std::move($1));
        $$.insert($$.end(), std::make_move_iterator($3.begin()), std::make_move_iterator($3.end()));
    }
    ;

selattr:
    MIN '(' ATTRNAME ')' { $$.function = SelectionPart::AggregateFunction::MINIMUM; $$.attribute = std::move($3); }
    | ATTRNAME { $$.function = SelectionPart::AggregateFunction::NONE; $$.attribute = std::move($1); }

frparams:
    frparam { $$.push_back(std::move($1)); }
    | frparam ',' frparams {
        $$.push_back(std::move($1));
        $$.insert($$.end(),
            std::make_move_iterator($3.begin()),
            std::make_move_iterator($3.end())
        );
    }
    ;

frparam:
    '('query')' AS TABLENAME { $$ = std::make_unique<FromQuery>(std::move($2), std::move($5)); }
    | TABLENAME { $$ = std::make_unique<FromTable>(std::move($1)); }
    ;

attrlist:
    ATTRNAME { $$ = {$1}; }
    | ATTRNAME ',' attrlist {$$ = std::move($3); $$.push_back(std::move($1)); }
    ;

whparams:
    predicate { $$ = std::move($1); }
    | '('whparams')' { $$ = std::move($2); }
    | whparams AND whparams { $$ = std::make_unique<ANDPredicate>(std::move($1), std::move($3)); }
    | whparams OR whparams  { $$ = std::make_unique<ORPredicate> (std::move($1), std::move($3)); }
    ;

predicate:
    ATTRNAME relation ATTRNAME { $$ = std::make_unique<AttributePredicate>($1, $3, $2); }
    | ATTRNAME relation INT { $$ = std::make_unique<ConstPredicate>($1, $3, $2); }
    | ATTRNAME relation STR { $$ = std::make_unique<ConstPredicate>($1, $3, $2); }
    | ATTRNAME IN '('query')' { $$ = std::make_unique<QueryPredicate>($1, std::move($4), true); }
    | ATTRNAME NOTIN '('query')' { $$ = std::make_unique<QueryPredicate>($1, std::move($4), false); }
    ;

relation:
    GR { $$ = Predicate::Relation::GREATER; }
    | EQ { $$ = Predicate::Relation::EQUAL; }
    | LE { $$ = Predicate::Relation::LESS; }
    ;

%%

void ToyDBMS::DBParser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
