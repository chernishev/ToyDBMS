#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ToyDBMS {

struct Value {
    enum class Type { INT, STR };
    Type type;
    int intval;
    std::string strval;

    Value(int i): type(Type::INT), intval(i) {}
    Value(std::string s): type(Type::STR), strval(s) {}
    Value(std::string v, Type type): type(type) {
        if(type == Type::INT) intval = std::stoi(v);
        else strval = v;
    }

    bool operator==(const Value &other) const {
        if(type != other.type) return false;
        switch(type){
        case Type::INT:
            return intval == other.intval;
        case Type::STR:
            return strval == other.strval;
        default: throw std::runtime_error("unknown value type");
        }
    }

    bool operator!=(const Value &other) const {
        return !(*this == other);
    }

    bool operator<(const Value &other) const {
        if(type != other.type)
            throw std::runtime_error("can't compare values of two different types");
        switch(type){
        case Type::INT:
            return intval < other.intval;
        case Type::STR:
            return strval < other.strval;
        default: throw std::runtime_error("unknown value type");
        }
    }

    bool operator>(const Value &other) const {
        if(type != other.type)
            throw std::runtime_error("can't compare values of two different types");
        switch(type){
        case Type::INT:
            return intval > other.intval;
        case Type::STR:
            return strval > other.strval;
        default: throw std::runtime_error("unknown value type");
        }
    }
};

struct Header : public std::vector<std::string> {
    using vector::vector;

    size_type index(std::string attr) const {
        for(size_type i = 0; i < size(); i++)
            if((*this)[i] == attr) return i;
        throw std::runtime_error("unknown attribute " + attr);
    }
};

struct Row {
    using size_type = std::vector<Value>::size_type;
    std::shared_ptr<Header> header;
    std::vector<Value> values;

    Row(){}

    Row(const std::shared_ptr<Header> &header, std::vector<Value> &&values)
        : header(header), values(std::move(values)) {}

    Row(const Row &other) = default;
    Row(Row &&other) = default;
    Row &operator=(const Row &other) = default;
    Row &operator=(Row &&other) = default;

    Value &operator[](size_type i){
        return values[i];
    }

    Value &operator[](std::string attr){
        return values[header->index(attr)];
    }

    const Value &operator[](size_type i) const {
        return values[i];
    }

    const Value &operator[](std::string attr) const {
        return values[header->index(attr)];
    }

    size_type size() const { return values.size(); }

    operator bool(){ return !values.empty(); }
};

inline std::ostream &operator<<(std::ostream &os, const Header &header){
    if(header.empty()) return os;
    for(Row::size_type i = 0; i < header.size() - 1; i++)
        os << header[i] << '\t';
    os << header.back();
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const Value &val){
    switch(val.type){
    case Value::Type::INT:
        return os << val.intval;
    case Value::Type::STR:
        return os << val.strval;
    default: throw std::runtime_error("unknown value type");
    }
}

inline std::ostream &operator<<(std::ostream &os, const Row &row){
    for(Row::size_type i = 0; i < row.size() - 1; i++)
        os << row.values[i] << '\t';
    os << row.values.back();
    return os;
}

}
