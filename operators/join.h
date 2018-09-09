#pragma once
#include <memory>
#include "operator.h"

namespace ToyDBMS {

class NLJoin : public Operator {
    std::unique_ptr<Operator> left, right;
    std::string left_attr, right_attr;
    Row current_left;
    std::shared_ptr<Header> header_ptr;
    Header::size_type left_index, right_index;

    Header construct_header(const Header &h1, const Header &h2){
        Header res {h1};
        res.insert(res.end(), h2.begin(), h2.end());
        return res;
    }
public:
    NLJoin(std::unique_ptr<Operator> left, std::unique_ptr<Operator> right,
           std::string left_attr, std::string right_attr)
        : left(std::move(left)), right(std::move(right)),
          left_attr(left_attr), right_attr(right_attr),
          header_ptr(std::make_shared<Header>(
            construct_header(this->left->header(), this->right->header()))
          ),
          left_index(this->left->header().index(left_attr)),
          right_index(this->right->header().index(right_attr)) {}

    const Header &header() override { return *header_ptr; }
    Row next() override;
    void reset() override;
};

}
