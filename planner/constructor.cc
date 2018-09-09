#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include "constructor.h"

#include "../operators/datasource.h"
#include "../operators/filter.h"
#include "../operators/join.h"

namespace ToyDBMS {

static const AttributePredicate *find_join(const Predicate &predicate){
    switch(predicate.type){
    case Predicate::Type::CONST:
        return nullptr;
    case Predicate::Type::ATTR:
        return dynamic_cast<const AttributePredicate*>(&predicate);
    case Predicate::Type::AND:
    {
        const auto &pred = dynamic_cast<const ANDPredicate&>(predicate);
        auto found_left  = find_join(*pred.left);
        auto found_right = find_join(*pred.right);
        if(found_left && found_right) throw std::runtime_error("only one join predicate is allowed for now");
        return found_left ? found_left : found_right;
    }
    default:
        throw std::runtime_error("encountered a predicate not yet supported");
    }
}

static void for_each_filter(const Predicate &predicate, std::function<void(const ConstPredicate&)> action){
    switch(predicate.type){
    case Predicate::Type::CONST:
        action(dynamic_cast<const ConstPredicate&>(predicate));
        break;
    case Predicate::Type::ATTR:
        return;
    case Predicate::Type::AND:
    {
        const auto &pred = dynamic_cast<const ANDPredicate&>(predicate);
        for_each_filter(*pred.left, action);
        for_each_filter(*pred.right, action);
        break;
    }
    default:
        throw std::runtime_error("encountered a predicate not yet supported");
    }
}

static std::string table_name(std::string attribute_name){
    return {attribute_name.begin(), std::find(attribute_name.begin(), attribute_name.end(), '.')};
}

struct Source {
    std::string table;
    std::unique_ptr<Predicate> predicate;
    std::unique_ptr<DataSource> datasource;

    Source(std::string table, std::unique_ptr<Predicate> predicate, std::unique_ptr<DataSource> ds)
        : table(std::move(table)), predicate(std::move(predicate)), datasource(std::move(ds)) {}

    std::unique_ptr<Operator> construct(){
        if(predicate)
             return std::make_unique<Filter>(
                 std::move(datasource), std::move(predicate)
             );
        else return std::move(datasource);
    }
};

std::unique_ptr<Operator> create_plan(const Query &query){
    std::vector<Source> sources;
    for(const auto &table : query.from){
        if(table->type == FromPart::Type::QUERY)
            throw std::runtime_error("queries in the FROM clause are not yet implemented");
        std::string table_name = dynamic_cast<const FromTable&>(*table).table_name;
        sources.emplace_back(
            table_name,
            std::unique_ptr<Predicate>{},
            std::make_unique<DataSource>("tables/" + table_name + ".csv")
        );
    }

    if(sources.size() < 1 || 2 < sources.size())
        throw std::runtime_error("only one or two tables can be used for now");

    if(query.where){
        for_each_filter(*query.where, [&sources](const auto &pred){
            std::string table = table_name(pred.attribute);
            bool found = false;
            for(auto &source : sources){
                if(source.table == table){
                    found = true;
                    if(!source.predicate)
                        source.predicate = std::make_unique<ConstPredicate>(pred);
                    else
                        source.predicate = std::make_unique<ANDPredicate>(
                            std::make_unique<ConstPredicate>(pred),
                            std::move(source.predicate)
                        );
                }
            }
            if(!found) throw std::runtime_error("couldn't find a source for a predicate");
        });

        auto join = find_join(*query.where);
        if(join){
            if(sources.size() == 1) throw std::runtime_error("no table to join with");

            std::string left = join->left, right = join->right;
            if(sources.front().table == left)
                std::swap(left, right);

            return std::make_unique<NLJoin>(
                sources[0].construct(), sources[1].construct(),
                left, right
            );
        }
    }

    if(sources.size() != 1)
        throw std::runtime_error("in the current state the system requires a join predicate when two tables are given");

    return sources.front().construct();
}

}
