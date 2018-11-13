#include "histogram.h"
#include <fstream>

namespace ToyDBMS {

Histogram Histogram::load(std::string filename, Value::Type type){
    std::ifstream file(filename);
    Histogram result;
    std::string min, max;
    size_t count;
    while(file >> min >> max >> count){
        Bucket bucket;
        bucket.min = Value{type, min};
        bucket.max = Value{type, max};
        bucket.count = count;
        result.buckets.push_back(std::move(bucket));
    }
    return result;
}

Histogram Histogram::intersect(const Histogram &other){
    return {};
}

size_t Histogram::num_elements(const Histogram &other){
    return 0;
}

}
