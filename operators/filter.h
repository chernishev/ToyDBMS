#pragma once
#include "operator.h"
#include "../parser/query.h"

namespace ToyDBMS {

class Filter : public Operator {
    std::unique_ptr<Operator> child;
    std::unique_ptr<Predicate> predicate;
public:
    Filter(std::unique_ptr<Operator> child, std::unique_ptr<Predicate> p)
        : child(std::move(child)), predicate(std::move(p)) {}

    const Header &header(){ return child->header(); }

    Row next() override {
        Row row;
        do {
            row = child->next();
            if(!row) return {};
        } while(!predicate->check(row));
        return row;
    }

    void reset() override {
        child->reset();
    }
};

}
