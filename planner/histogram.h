#pragma once
#include "../operators/row.h"
#include <string>

namespace ToyDBMS {

struct Bucket {
    Value min, max;
    size_t count;
};

struct Histogram {
    std::vector<Bucket> buckets;

    static Histogram load(std::string filename, Value::Type type);

    Histogram intersect(const Histogram &other) const;
    size_t num_elements() const;
};

}
