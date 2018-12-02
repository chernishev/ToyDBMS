#include <cstdlib>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

#include "constructor.h"
#include "../operators/print.h"
#include "../util/config.h"

using namespace ToyDBMS;

int main(int argc, char* argv[]){
    if(argc == 3){
        Config::available_memory = std::atoi(argv[1]);
        Config::block_size       = std::atoi(argv[2]);
    } else {
        std::cout << "You need to specify the amount of available memory and block size\n";
        return 1;
    }
    while(char *line = readline("> ")){
        if(*line == '\0') continue;
        try {
            const Query &q = Query::parse(line);
            Print p(create_plan(q));
            while(p.next());
        } catch(std::exception &e){
            std::cerr << e.what() << '\n';
        }
        add_history(line);
        free(line);
    }
    return 0;
}
