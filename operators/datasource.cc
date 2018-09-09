#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "datasource.h"

namespace ToyDBMS {

DataSource::DataSource(std::string filename): file(filename) {
    if(!file.good())
        throw std::runtime_error("failed to open file: " + filename);

    std::size_t dot_pos = filename.rfind('.');
    std::size_t sep_pos = filename.find_last_of("/\\");

    if(dot_pos == std::string::npos || sep_pos == std::string::npos || dot_pos < sep_pos)
        throw std::runtime_error("invalid filename: " + filename);

    std::string table_name = filename.substr(sep_pos + 1, dot_pos - sep_pos);

    std::string header_line;
    std::getline(file, header_line);

    header_ptr = std::make_shared<Header>();

    std::istringstream ss {header_line};
    std::string part;
    while(std::getline(ss, part, ',')){
        std::string attr = table_name;
        attr.append(part.begin() + 2, part.end());
        file_header.emplace_back(attr, part[0] == 'i' ? Value::Type::INT : Value::Type::STR);
        header_ptr->push_back(std::move(attr));
    }

    after_header = file.tellg();
}

Row DataSource::next(){
    std::string line, part;
    if(!std::getline(file, line)) return {};
    std::istringstream ss {line};

    std::vector<Value> values;
    for(auto &field : file_header){
        std::getline(ss, part, ',');
        switch(field.second){
        case Value::Type::INT:
            values.emplace_back(std::stoi(part));
            break;
        case Value::Type::STR:
            values.emplace_back(part);
            break;
        }
    }
    return {header_ptr, std::move(values)};
}

void DataSource::reset(){
    file.clear();
    file.seekg(after_header);
}

}
