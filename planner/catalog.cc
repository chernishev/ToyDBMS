#include "catalog.h"
#include <fstream>

std::vector<std::string> split(std::string str){
    std::vector<std::string> parts;
    auto end = str.find_first_not_of(" \t");
    auto start = end;
    while(start != std::string::npos){
        end = str.find_first_of(" \t", start);
        parts.emplace_back(str, start, end - start);
        start = str.find_first_not_of(" \t", end);
    }

    return parts;
}

namespace ToyDBMS {

Catalog::Catalog(){
    auto string_to_order = [](const std::string &order){
        if(order == "ASC"){
            return Column::SortOrder::ASC;
        } else if(order == "DESC"){
            return Column::SortOrder::DESC;
        } else if(order == "UNSORTED"){
            return Column::SortOrder::UNSORTED;
        } else if(order == "UNKNOWN"){
            return Column::SortOrder::UNKNOWN;
        } else throw std::runtime_error("invalid sort order " + order);
    };
    std::ifstream file("catalog.txt");
    std::string line;
    std::unordered_map<std::string, Table>::iterator current_table;
    while(getline(file, line)){
        auto parts = split(line);
        if(parts.size() == 2){
            current_table = tables.emplace(std::piecewise_construct,
                std::forward_as_tuple(parts[0]),
                std::forward_as_tuple(parts[0], std::stoi(parts[1]))
            ).first;
        } else if(parts.size() == 6){
            bool is_int = parts[1] == "INT";
            current_table->second.addColumn(
                parts[0],
                is_int ? Value::Type::INT : Value::Type::STR,
                string_to_order(parts[2]),
                parts[3] == "UNIQUE",
                is_int ? Value {std::stoi(parts[4])} : Value {parts[4]},
                is_int ? Value {std::stoi(parts[5])} : Value {parts[5]}
            );
        } else throw std::runtime_error("wrong catalog format");
    }
}

}
