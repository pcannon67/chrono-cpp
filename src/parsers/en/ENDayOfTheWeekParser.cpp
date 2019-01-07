#ifndef ENDATEOFWEEKPARSER_HPP
#define ENDATEOFWEEKPARSER_HPP

#include "src/parsers/parsers.h"

#define PATTERN "(\\W|^)(on\\s*)?(?:(this|last|next)\\s*)?(Sunday|Sun|Monday|Mon|Tuesday|Tues|Tue|Wednesday|Wed|Thursday|Thu(?:rs|r)?|Friday|Fri|Saturday|Sat)(?=\\W|$)"

const int PREFIX_GROUP  = 2;
const int WEEKDAY_GROUP = 3;
const int POSTFIX_GROUP = 4;
static std::map DAYS_OFFSET = { 'sunday': 0, 'sun': 0, 'monday': 1, 'mon': 1,
                                'tuesday': 2, 'tues':2, 'tue':2, 'wednesday': 3, 'wed': 3,
                                'thursday': 4, 'thurs':4, 'thur': 4, 'thu': 4,'friday': 5,
                                'fri': 5,'saturday': 6, 'sat': 6};

class ENDayOfWeekParser : public Parser {
public:
    ENDayOfWeekParser() : Parser(false, std::regex(PATTERN, std::regex::icase)) { }

    parse::ParsedResult updateParsedComponent(parse::ParsedResult res,
            posix_time::ptime& ref, int offset, std::string modifier) {

        posix_time::ptime start{ref};
        bool start_fixed{false};
        int count=0;
        gregorian::date resOffset;

        if(!modifier.compare("last") or !modifier.compare("past")) {
            resOffset{previous_weekday(start, gregorian::greg_weekday(offset))};
            while( resOffset.week_number() == start.date().week_number() ) {
                // avoid returning current day or offset from current week.
                // See: https://stackoverflow.com/a/30470686
                resOffset = date_time::next_weekday(start - gregorian::days(count),
                        gregorian::greg_weekday(offset));
                count++;
            }
        }
        else if(!modifier.compare("next")) {
            resOffset{date_time::next_weekday(start, gregorian::greg_weekday(offset))};
            while( resOffset.week_number() == start.date().week_number() ) {
                // avoid returning current day or offset from current week.
                // See: https://stackoverflow.com/a/30470686
                resOffset = date_time::next_weekday(start + gregorian::days(count), gregorian::greg_weekday(offset));
                count++;
            }
        }
        else if(!modifier.compare("this") or modifier.empty()) {
            if(offset == ref.date().day_of_week().as_number()){
                resOffset{ref.date()};
            }
            else if(offset < ref.date().day_of_week().as_number() ) {
                gregorian::first_day_of_the_week_before fdb{offset};
                resOffset{fdb.get_date(ref)};
            }
            else {
                gregorian::first_day_of_the_week_after fda{offset};
                resOffset{fda.get_date(ref)};
            }
        }
        else {
            resOffset{gregorian::not_a_date_time};
        }

        if(resOffset.is_special()) {
            return res;
        }
        else {
            res.startDate.implyComponent("mday", resOffset.day());
            res.startDate.implyComponent("month", resOffset.month());
            res.startDate.implyComponent("year", resOffset.year());
        }
        return res;
    }

    parse::ParsedResult extract(std::string tx, std::smatch match, posix_time::ptime& ref) {
        std::string text = match[0].str().substr(match[1].length());
        int idx = match.position(0) + match[1].length();

        parse::ParsedResult result = parse::ParsedResult(ref, idx, text);

        std::string dayOfWeek = match[WEEKDAY_GROUP];

        int offset = DAYS_OFFSET[dayOfWeek];
        if(offset < 0 or offset > 6) {
            return result;
        }

        std::string prefix  = match[PREFIX_GROUP];
        std::string postfix = match[POSTFIX_GROUP];
        std::string norm;

        if(prefix.empty())
            norm{postfix};
        else if(postfix.empty())
            norm{prefix};
        else
            norm{""};

        // updateParsedComponent
        Result _result = updateParsedComponent(result, ref, offset, norm);

        // set tag
        _result.setTag(utils::ENDayOfTheWeekParser);

    }
};

#undef PATTERN

#endif