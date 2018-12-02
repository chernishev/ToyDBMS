#include <cstdlib>
#include <stdexcept>
#include "parser/query.h"
#include "planner/constructor.h"
#include "operators/print.h"
#include "util/config.h"

using namespace ToyDBMS;

int main(int argc, char* argv[]){
    if(argc == 3){
        Config::available_memory = std::atoi(argv[1]);
        Config::block_size       = std::atoi(argv[2]);
    } else {
        std::cout << "You need to specify the amount of available memory and block size\n";
        return 1;
    }
    try {
        Print p {create_plan(Query::parse(std::cin))};
        while(p.next());
        return 0;
    } catch(std::exception &e){
        std::cerr << e.what();
        return 1;
    }
}
