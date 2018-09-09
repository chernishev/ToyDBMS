#include <iostream>
#include "query.h"

int main(const int argc, const char **argv){
    if(argc == 2){
        try {
            ToyDBMS::Query::parse_file(argv[1]).print();
        } catch(std::exception &e){
            std::cerr << e.what() << '\n';
        }
    } else {
        std::cout << "Need filename to read from!\n";
    }
}
