#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

#include "constructor.h"
#include "../operators/print.h"

using namespace ToyDBMS;

int main(){
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
