#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <regex>
using namespace std;
/////////////////////////////////////////////////////////
shared_ptr<QueryBase> QueryBase::factory(const string &s)
{
    vector<string> splitResult;
    istringstream iss(s);
    for (string s; iss >> s;)
        splitResult.push_back(s);
    if (splitResult.size() == 1)
    {
        return std::shared_ptr<QueryBase>(new WordQuery(splitResult[0]));
    }
    else if (splitResult.size() == 3)
    {
        const string type = splitResult[0];
        const string word1 = splitResult[1];
        const string word2 = splitResult[2];
        if (type == "AND")
            return std::shared_ptr<QueryBase>(new AndQuery(word1, word2));
        else if (type == "OR")
            return std::shared_ptr<QueryBase>(new OrQuery(word1, word2));
        else if (type == "AD")
            return std::shared_ptr<QueryBase>(new AdjacentQuery(word1, word2));
        else
            throw invalid_argument("Unrecognized search \n");
    }
    else
    {
        throw invalid_argument("Unrecognized search \n");
    }
}
/////////////////////////////////////////////////////////
QueryResult AndQuery::eval(const TextQuery &text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();
    set_intersection(left_result.begin(), left_result.end(),
                     right_result.begin(), right_result.end(),
                     inserter(*ret_lines, ret_lines->begin()));
    return QueryResult(rep(), ret_lines, left_result.get_file());
}

QueryResult OrQuery::eval(const TextQuery &text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>(left_result.begin(), left_result.end());
    ret_lines->insert(right_result.begin(), right_result.end());
    return QueryResult(rep(), ret_lines, left_result.get_file());
}
/////////////////////////////////////////////////////////

QueryResult AdjacentQuery::eval(const TextQuery &text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();
    auto it_left = left_result.begin();
    auto it_right = right_result.begin();
    int prev = 0; //prev =  1 left moved pre=2 tight moved
    while (it_left != left_result.end() && it_right != right_result.end())
    {
        int lLeft = *it_left;
        int lRight = *it_right;
        // cout << prev << endl;
        if (lLeft + 1 == lRight)
        {
            ret_lines->insert(lLeft);
            ret_lines->insert(lRight);
            prev = 1;
            ++it_left;
        }
        else if (lLeft + 1 < lRight)
        {
            ++it_left;
        }
        else if (lLeft == lRight + 1)
        {
            ret_lines->insert(lLeft);
            ret_lines->insert(lRight);
            prev = 2;
            ++it_right;
        }
        else if (lLeft > lRight + 1)
        {
            ++it_right;
        }
        else if (lLeft == lRight)
        {
            if (prev == 1)
            {
                ++it_left;
            }
            else if (prev == 2)
            {
                ++it_right;
            }
        }
    }

    return QueryResult(rep(), ret_lines, left_result.get_file());
}

std::ostream &print(std::ostream &os, const QueryResult &qr)
{
    if (qr.sought.find("AD") == string::npos)
    {
        os << "\"" << qr.sought << "\""
           << " occurs " << qr.lines->size() << " times:" << std::endl;
        for (auto num : *qr.lines)
        {
            os << "\t(line " << num + 1 << ") "
               << *(qr.file->begin() + num) << std::endl;
        }
    }
    else
    {

        int occu = 0;
        int is_odd = 0;
        int duel = 0;
        if (qr.lines->size() % 2 == 0) //number of adj lines are even
        {
            if (qr.lines->size() == 2)
                duel = 1;
            occu = qr.lines->size() / 2;
        }
        else
        {
            occu = qr.lines->size() / 2 + 1; // number of adj lines = odd which means there is a line that is adj of 2 different lines.
            is_odd = 1;
        }
        os << "\"" << qr.sought << "\""
           << " occurs " << occu << " times:" << std::endl;
        int pair = 0;
        int count = 0;
        for (auto num : *qr.lines)
        {
            pair++;
            count++;
            os << "\t(line " << num + 1 << ") "
               << *(qr.file->begin() + num) << std::endl;
            if (pair == 2 && duel == 0 && count != 6)
            {
                os << endl;
                pair = 0;
            }
            if (count == 2 && is_odd)
            {
                os << "\t(line " << num + 1 << ") "
                   << *(qr.file->begin() + num) << std::endl;
            }
        }
    }

    return os;
}
/////////////////////////////////////////////////////////
