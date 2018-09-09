#pragma once
#include "row.h"

namespace ToyDBMS {

class Operator {
public:
    virtual ~Operator(){}
    virtual const Header &header() = 0;
    virtual Row  next()  = 0;
    virtual void reset() = 0;
};

}
