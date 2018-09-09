#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../operators/row.h"

namespace ToyDBMS {
    class Query;

    // *** PREDICATES ***
    struct Predicate {
        enum class Type { CONST, ATTR, INQUERY, AND, OR };
        enum class Relation { LESS, EQUAL, GREATER };

        Type type;

        Predicate(Type type): type(type) {}

        virtual ~Predicate(){}
        virtual void print() = 0;
        virtual bool check(const Row &row) = 0;
    };

    struct ConstPredicate : public Predicate {
        std::string attribute;
        Relation relation;
        Value value;

        ConstPredicate(std::string attr, int val, Relation rel)
            : Predicate(Type::CONST), attribute(attr), relation(rel), value(val) {}

        ConstPredicate(std::string attr, std::string val, Relation rel)
            : Predicate(Type::CONST), attribute(attr), relation(rel), value(val) {}

        void print() override;
        bool check(const Row &row) override;
    };

    struct AttributePredicate : public Predicate {
        std::string left, right;
        Relation relation;

        AttributePredicate(std::string left, std::string right, Relation rel)
            : Predicate(Type::ATTR), left(std::move(left)), right(std::move(right)), relation(rel) {}

        void print() override;
        bool check(const Row &row) override;
    };

    struct QueryPredicate : public Predicate {
        std::string attribute;
        std::unique_ptr<Query> query;
        bool in; // true if IN, false if NOT IN

        QueryPredicate(std::string attr, std::unique_ptr<Query> query, bool in)
            : Predicate(Type::INQUERY), attribute(attr),
              query(std::move(query)), in(in) {}

        void print() override;
        bool check(const Row &row) override;
    };

    struct ANDPredicate : public Predicate {
        std::unique_ptr<Predicate> left, right;

        ANDPredicate(std::unique_ptr<Predicate> left, std::unique_ptr<Predicate> right)
            : Predicate(Type::AND), left(std::move(left)), right(std::move(right)) {}

        void print() override;
        bool check(const Row &row) override;
    };

    struct ORPredicate : public Predicate {
        std::unique_ptr<Predicate> left, right;

        ORPredicate(std::unique_ptr<Predicate> left, std::unique_ptr<Predicate> right)
            : Predicate(Type::AND), left(std::move(left)), right(std::move(right)) {}

        void print() override;
        bool check(const Row &row) override;
    };

    // *** SELECTION ***

    struct SelectionPart {
        enum class AggregateFunction { NONE, MINIMUM };

        AggregateFunction function;
        std::string attribute;
    };

    struct SelectionClause {
        enum class Type { ALL, COUNT, LIST };

        Type type;
        std::vector<SelectionPart> attrs;
    };

    // *** FROM ***

    struct FromPart {
        enum class Type { TABLE, QUERY };

        Type type;
        std::string alias;

        FromPart(Type type, std::string alias = "")
            : type(type), alias(std::move(alias)) {}

        virtual void print() = 0;
    };

    struct FromTable : public FromPart {
        std::string table_name;

        FromTable(std::string table)
            : FromPart(Type::TABLE), table_name(std::move(table)) {}

        void print() override;
    };

    struct FromQuery : public FromPart {
        std::unique_ptr<Query> query;

        FromQuery(std::unique_ptr<Query> query, std::string alias)
            : FromPart(Type::QUERY, std::move(alias)),
              query(std::move(query)) {}

        void print() override;
    };

    // *** QUERY ***

    struct Query {
        bool distinct;
        SelectionClause selection;
        std::vector<std::unique_ptr<FromPart>> from;
        std::unique_ptr<Predicate> where;
        std::vector<std::string> groupby;
        std::vector<std::string> orderby;

        void print();
        static Query parse(std::istream &stream);
        static Query parse(std::string query);
        static Query parse_file(std::string filename);
    };
}
