#pragma once
#include <unordered_map>
#include "../operators/row.h"

namespace ToyDBMS {

struct Column {
    enum class SortOrder { ASC, DESC, UNSORTED, UNKNOWN };
    std::string name;
    Value::Type type;
    SortOrder order;
    bool unique;
    Value min, max;
    Column(std::string name, Value::Type type, Column::SortOrder order, bool unique, Value min, Value max)
        : name(name), type(type), order(order), unique(unique), min(min), max(max) {}
};

struct Table {
    std::string name;
    size_t rows;
    std::unordered_map<std::string, Column> columns;
    Table(std::string name, size_t rows): name(name), rows(rows) {}
    void addColumn(std::string name, Value::Type type, Column::SortOrder order, bool unique, Value min, Value max){
        columns.emplace(std::piecewise_construct,
            std::forward_as_tuple(name),
            std::forward_as_tuple(name, type, order, unique, min, max)
        );
    }
    const Column &operator[](const std::string &str){
        return columns.at(str);
    }
};

struct Catalog {
    std::unordered_map<std::string, Table> tables;
    Catalog();
    const Table &operator[](const std::string &str){
        return tables.at(str);
    }
    const Column &getColumn(const std::string &fullname){
        auto sep = fullname.find('.');
        std::string table{fullname, 0, sep}, column{fullname, sep + 1};
        return tables.at(table).columns.at(column);
    }
};

}
