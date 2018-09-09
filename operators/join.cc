#include <algorithm>
#include <iterator>
#include "join.h"

namespace ToyDBMS {

Row NLJoin::next(){
    if(!current_left)
        current_left = left->next();

    if(!current_left) return {};

    Row current_right = right->next();
    while(true){
        if(!current_right){
            current_left = left->next();
            if(!current_left) return {};

            right->reset();
            current_right = right->next();
            if(!current_right) return {};
        }

        if(current_left[left_index] != current_right[right_index]){
            current_right = right->next();
            continue;
        }

        std::vector<Value> values = current_left.values;
        values.insert(values.end(),
                      std::make_move_iterator(current_right.values.begin()),
                      std::make_move_iterator(current_right.values.end()));

        return {header_ptr, std::move(values)};
    }
}

void NLJoin::reset(){
    left->reset();
    right->reset();
    current_left = {};
}

}
