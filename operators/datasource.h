#pragma once
#include <memory>
#include <fstream>
#include <string>

#include "operator.h"

namespace ToyDBMS {

class DataSource : public Operator {
    std::ifstream file;
    std::streampos after_header;
                        // attr name ,  attr type
    std::vector<std::pair<std::string, Value::Type>> file_header;
    std::shared_ptr<Header> header_ptr;
public:
    DataSource(std::string filename);

    const Header &header() override { return *header_ptr; }
    Row next() override;
    void reset() override;
};

}
