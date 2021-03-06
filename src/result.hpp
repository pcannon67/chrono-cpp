#ifndef RESULT_H
#define RESULT_H

#include <iostream>
#include <ctime>
#include <unordered_map>
#include <string>
#include "src/utils/utils.hpp"

typedef std::unordered_map<std::string, std::pair<bool, int> > Components;

namespace parse {
    class ParsedComponents;
    class ParsedResult;
    ParsedComponents isAfter(ParsedComponents& );
}

class parse::ParsedComponents {
private:
    Components knownValues;
    Components impliedValues;

public:
    ParsedComponents();
    explicit ParsedComponents(const Components& );
    ParsedComponents(Components&, Components&);
    ParsedComponents(const ParsedComponents&);
    ~ParsedComponents();
    ParsedComponents& operator=(const ParsedComponents& );

    void implyComponent(std::string, int);
    bool isCertain(std::string);
    bool isPossibleDate();
    posix_time::ptime date();

    int getYear();
    int getMonth();
    int get_mDay();
    int get_wDay();
    int getHour();
    int getMinute();
    int getSeconds();
    int getTimeZoneOffset();
    // int getDaylightSavings();

    void setYear(int);
    void setMonth(int);
    void set_mDay(int);
    void set_wDay(int);
    void setHour(int);
    void setMinute(int);
    void setSeconds(int);
    void setTimeZoneOffset(int);

};


class parse::ParsedResult {
private:
    bool __end;
protected:
    posix_time::ptime anchor;
    unsigned index;
    std::string text; /// todo: add accessor for text to aid testing
    utils::Tags tags;

public:
    ParsedResult();
    ParsedResult(posix_time::ptime, long, std::string);
    ParsedResult(const ParsedResult& pr);
    ~ParsedResult();
    // ParsedResult& operator=(ParsedResult&);

    ParsedComponents startDate, endDate;    // todo: make protected & make accessors and mutators

    bool isEmpty() const;
    void setTag(utils::Modifiers);
    bool hasPossibleDates();
    std::string toDate();
    unsigned getIndex() const;
    bool getTag(utils::Modifiers);
    void setIndex(int idx);
    void setText(std::string);
    size_t textLength() const;
    bool end() const;
    void makeEndDateValid();
    parse::ParsedResult& operator=(parse::ParsedResult);
};

typedef std::vector<parse::ParsedResult> Result;


#endif
