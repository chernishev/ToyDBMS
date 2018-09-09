#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "query.h"

#include "dbscanner.h"
#include "dbparser.tab.hh"

namespace ToyDBMS {

void ConstPredicate::print(){
    std::cout << attribute;
    switch(relation){
    case Relation::GREATER:
        std::cout << " > ";
        break;
    case Relation::EQUAL:
        std::cout << " = ";
        break;
    case Relation::LESS:
        std::cout << " < ";
        break;
    }
    std::cout << value << '\n';
}

bool ConstPredicate::check(const Row &row){
    const Value &row_val = row[attribute];
    switch(relation){
    case Relation::LESS:
        return row_val <  value;
    case Relation::EQUAL:
        return row_val == value;
    case Relation::GREATER:
        return row_val >  value;
    default: throw std::runtime_error("unknown value type");
    }
}

void AttributePredicate::print(){
    std::cout << left;
    switch(relation){
    case Relation::GREATER:
        std::cout << " > ";
        break;
    case Relation::EQUAL:
        std::cout << " = ";
        break;
    case Relation::LESS:
        std::cout << " < ";
        break;
    }
    std::cout << right << '\n';
}

bool AttributePredicate::check(const Row &row){
    throw std::runtime_error("not implemented");
}

void QueryPredicate::print(){
    std::cout << attribute << (in ? " IN " : " NOT IN ") << "{\n";
    query->print();
    std::cout << "}\n";
}

bool QueryPredicate::check(const Row &row){
    throw std::runtime_error("not implemented");
}

void ANDPredicate::print(){
    std::cout << "{\n";
    left->print();
    std::cout << "} AND {\n";
    right->print();
    std::cout << "}\n";
}

bool ANDPredicate::check(const Row &row){
    return left->check(row) && right->check(row);
}

void ORPredicate::print(){
    std::cout << "{\n";
    left->print();
    std::cout << "} OR {\n";
    right->print();
    std::cout << "}\n";
}

bool ORPredicate::check(const Row &row){
    return left->check(row) || right->check(row);
}

void FromTable::print(){
    std::cout << table_name << '\n';
}

void FromQuery::print(){
    std::cout << "(\n";
    query->print();
    std::cout << ") AS " << alias << '\n';
}

void Query::print(){
    std::cout << (distinct ? "DISTINCT" : "NOT DISTINCT") << '\n';

    std::cout << "ATTRIBUTES:\n";
    switch(selection.type){
    case SelectionClause::Type::ALL:
        std::cout << "*\n";
        break;
    case SelectionClause::Type::COUNT:
        std::cout << "COUNT(*)\n";
        break;
    case SelectionClause::Type::LIST:
        for(auto &part : selection.attrs){
            switch(part.function){
            case SelectionPart::AggregateFunction::NONE:
                std::cout << part.attribute << '\n';
                break;
            case SelectionPart::AggregateFunction::MINIMUM:
                std::cout << "MIN(" << part.attribute << ")\n";
                break;
            }
        }
        break;
    }

    std::cout << "FROM:\n";
    for(auto &part : from){
        part->print();
    }

    if(where){
        std::cout << "PREDICATE:\n";
        where->print();
    }

    if(!groupby.empty()){
        std::cout << "GROUPBY:\n";
        for(auto &attr : groupby) std::cout << attr << '\n';
    }

    if(!orderby.empty()){
        std::cout << "ORDERBY:\n";
        for(auto &attr : orderby) std::cout << attr << '\n';
    }
}

Query Query::parse(std::istream &stream){
    Query q;
    DBScanner scanner(&stream);
    DBParser  parser(scanner, q);

    if(parser.parse()) throw std::runtime_error("parsing failed");
    return q;
}

Query Query::parse(std::string query){
    std::stringstream stream {query};
    return parse(stream);
}

Query Query::parse_file(std::string filename){
    std::ifstream infile(filename);
    if(!infile.good()){
        std::cout<< "File-related problem, exiting...";
        exit(EXIT_FAILURE);
    }
    return parse(infile);
}

} // end namespace
