#include <stdexcept>
#include "parser/query.h"
#include "planner/constructor.h"
#include "operators/print.h"

using namespace ToyDBMS;

int main(){
    try {
        Print p {create_plan(Query::parse(std::cin))};
        while(p.next());
        return 0;
    } catch(std::exception &e){
        std::cerr << e.what();
        return 1;
    }
}
