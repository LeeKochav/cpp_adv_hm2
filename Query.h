#ifndef QUERY_H
#define QUERY_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <memory>
#include <regex>
#include "TextQuery.h"
#include "QueryResult.h"

class QueryBase {
public:
    using line_no = TextQuery::line_no;
    virtual ~QueryBase() = default;
    virtual QueryResult eval(const TextQuery&) const = 0; 
    virtual std::string rep() const = 0;
    static std::shared_ptr<QueryBase> factory(const std::string& s);
};

class WordQuery: public QueryBase {
public:
    WordQuery(const std::string &s): query_word(s) { }
    QueryResult eval(const TextQuery &t) const
        { return t.query(query_word); }
    std::string rep() const { return query_word; }
private:
    std::string query_word;
};
   
class AndQuery: public QueryBase {
public:
    AndQuery(const std::string &left_string, const std::string &right_string):
        left_query(left_string), right_query(right_string) {}
    QueryResult eval(const TextQuery&) const;
    std::string rep() const { return "AND " + left_query + " " + right_query; }
protected:	
    std::string left_query, right_query;
};

class OrQuery: public QueryBase {
public:
    OrQuery(const std::string &left_string, const std::string &right_string):
        left_query(left_string), right_query(right_string) {}
    QueryResult eval(const TextQuery&) const;
    std::string rep() const { return "OR " + left_query + " " + right_query; }
private:
	std::string left_query, right_query;
};

class AdjacentQuery: public QueryBase {
public:
    AdjacentQuery(const std::string &left_string, const std::string &right_string):
        left_query(left_string), right_query(right_string) {}
    QueryResult eval(const TextQuery&) const;
    std::string rep() const { return "AD " + left_query + " " + right_query; }
private:
	std::string left_query, right_query;
};
#endif
