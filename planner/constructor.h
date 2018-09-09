#pragma once
#include <memory>
#include "../parser/query.h"
#include "../operators/operator.h"

namespace ToyDBMS {

std::unique_ptr<Operator> create_plan(const Query &query);

}
