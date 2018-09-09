#pragma once
#include <iostream>
#include <memory>
#include "operator.h"

namespace ToyDBMS {

class Print : public Operator {
    std::unique_ptr<Operator> child;
    bool first = true;
public:
    Print(std::unique_ptr<Operator> child): child(std::move(child)) {}

    const Header &header() override { return child->header(); }

    Row next() override {
        if(first){
            std::cout << header() << '\n';
            first = false;
        }

        Row row = child->next();
        if(!row) return {};

        std::cout << row << '\n';
        return row;
    }

    void reset() override {
        child->reset();
    }
};

}
