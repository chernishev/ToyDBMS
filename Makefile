CXX = g++
CXXFLAGS = -Wno-deprecated-register -O0 -g -Wall -std=c++14 #-DDBSCANDEBUG

PARSEROBJ   = parser/parser.o parser/lexer.o parser/query.o
OPERATOROBJ = operators/datasource.o operators/join.o
PLANNEROBJ  = planner/constructor.o

all: parsertestexe plannertestexe testexe

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $*.cc

parser/query.o: parser/parser.o

parser/parser.o: parser/dbparser.yy
	cd parser; bison -Werror=conflicts-sr -d -v -t dbparser.yy
	patch parser/dbparser.tab.hh parser/patch
	$(CXX) $(CXXFLAGS) -c -o $@ parser/dbparser.tab.cc

parser/lexer.o: parser/dblexer.l parser/parser.o
	flex --outfile=parser/dblexer.yy.cc $<
	$(CXX) $(CXXFLAGS) -c -o $@ parser/dblexer.yy.cc

testexe: main.cc $(PARSEROBJ) $(OPERATOROBJ) $(PLANNEROBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

parsertestexe: parser/parsertest.cc $(PARSEROBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

plannertestexe: planner/test.cc $(PARSEROBJ) $(OPERATOROBJ) $(PLANNEROBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lreadline

clean:
	rm -f parsertestexe plannertestexe testexe
	rm -f $(PARSEROBJ) $(OPERATOROBJ) $(PLANNEROBJ)
	rm -f $(addprefix parser/, dblexer.yy.cc dbparser.tab.cc dbparser.tab.hh \
		stack.hh location.hh position.hh dbparser.output)

test: testexe
	cd tests; ./run_all.sh

.PHONY: all clean test
