#include "catalog.h"

using namespace ToyDBMS;

int main(){
    Catalog catalog;
    for(const auto &kv : catalog.tables){
        const Table &table = kv.second;
        std::cout << table.name << ' ' << table.rows << '\n';
        for(const auto &kv : kv.second.columns){
            const Column &column = kv.second;
            std::cout << "    " << column.name << ' ' << (column.unique ? "unique" :"not unique")
                      << ' '    << column.min  << ' ' << column.max << '\n';
        }
    }
    const Column &column = catalog.getColumn("employees.manager");
    std::cout << column.name << ' ' << column.min << ' ' << column.max << '\n';
    return 0;
}
