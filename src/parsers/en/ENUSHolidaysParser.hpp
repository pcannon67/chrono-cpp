
#ifndef ENHOLIDAYPARSER_HPP
#define ENHOLIDAYPARSER_HPP

#include "src/parsers/parsers.hpp"

#define PATTERN "(\\W|^)(Independence|President\\'s|New\\s*Year\\'s(?:\\s*eve)?|" \
                "Columbus|MLK|Martin\\s*Luther\\s*King\\s*(?:Junior|Jr|Jnr)|" \
                "Thanksgiving|Memorial|Labor|Veteran(?:s|\\'s)|Christmas\\s*(?:eve)?|" \
                "black\\s*Friday)\\s*(?:day)?(?=\\W|$)"


class ENHolidayParser : public Parser {
public:
    ENHolidayParser() : Parser(false, std::regex(PATTERN, std::regex::icase)) { }

    parse::ParsedResult extract(std::string tx, std::smatch match, posix_time::ptime& ref, long idx)
    override {
        std::string text = match.str(0).substr(match.length(1));
        parse::ParsedResult result = parse::ParsedResult(ref, idx, text);
        gregorian::date day;

        // starting with fixed dates
        if(text.find("independence") != std::string::npos) {
            result.startDate.setYear(ref.date().year());
            result.startDate.setMonth(7);
            result.startDate.set_mDay(4);
        }
        else if(text.find("new year") != std::string::npos) {
            result.startDate.setYear(ref.date().year());
            result.startDate.setMonth(1);
            result.startDate.set_mDay(1);

            if(text.find("eve") != std::string::npos) {
                // this or last year??
                // result.startDate.setYear(ref.date().year() - 1);
                result.startDate.setMonth(12);
                result.startDate.set_mDay(31);
            }
        }
        else if(text.find("veteran") != std::string::npos) {
            result.startDate.setYear(ref.date().year());
            result.startDate.setMonth(11);
            result.startDate.set_mDay(11);
        }
        else if(text.find("christmas") != std::string::npos) {
            result.startDate.setYear(ref.date().year());
            result.startDate.setMonth(12);
            result.startDate.set_mDay(25);

            if(text.find("eve") != std::string::npos) {
                result.startDate.set_mDay(24);
            }
        }
        else if(text.find("mlk") != std::string::npos or
                text.find(utils::toLowerCase("Martin Luther King")) != std::string::npos) {
            //MLK day is the third Monday in Jan
            ndow thirdMondayOfJan(ndow::third, gregorian::Monday, gregorian::Jan);
            day = thirdMondayOfJan.get_date(ref.date().year());

            result.startDate.setYear(day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day());
            result.startDate.set_wDay(day.day_of_week());
        }
        else if(text.find("labor") != std::string::npos) {
            //US labor day is first Monday in Sept
            ndow firstMondayInSep(ndow::first, gregorian::Monday, gregorian::Sep);
            day = firstMondayInSep.get_date(ref.date().year());

            result.startDate.setYear(day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day());
            result.startDate.set_wDay(day.day_of_week());
        }
        else if(text.find("president") != std::string::npos) {
            //in the US president's day is the second Tuesday in Feb
            ndow thirdMondayInFeb(ndow::third, gregorian::Monday, gregorian::Feb);
            day = thirdMondayInFeb.get_date(ref.date().year());

            result.startDate.setYear(day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day());
            result.startDate.set_wDay(day.day_of_week());
        }
        else if(text.find("thanksgiving") != std::string::npos) {
            //in the US president's day is the second Tuesday in Feb
            ndow fourthThurInNov(ndow::fourth, gregorian::Thursday, gregorian::Nov);
            day = fourthThurInNov.get_date(ref.date().year());

            result.startDate.setYear(day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day());
            result.startDate.set_wDay(day.day_of_week());
        }
        else if(text.find("black") != std::string::npos) {
            //black friday occurs the day after thanksgiving
            ndow fourthThurInNov(ndow::fourth, gregorian::Thursday, gregorian::Nov);
            gregorian::date thanksgiving = fourthThurInNov.get_date(ref.date().year());
            day = thanksgiving + gregorian::days(1);
            // could have made the above shorter but it's easier to understand this way

            result.startDate.setYear (day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day() + 1);
            result.startDate.set_wDay(day.day_of_week() + 1);
        }
        else if(text.find("memorial") != std::string::npos) {
            // Memorial day is observed on the last Monday of May
            ldow lastMondayInMay(gregorian::Monday, gregorian::May);
            day = lastMondayInMay.get_date(ref.date().year());

            result.startDate.setYear(day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day());
            result.startDate.set_wDay(day.day_of_week());
        }
        else if(text.find("columbus") != std::string::npos) {
            // Memorial day is observed on the last Monday of May
            ndow secondMondayInOct(ndow::second, gregorian::Monday, gregorian::Oct);
            day = secondMondayInOct.get_date(ref.date().year());

            result.startDate.setYear(day.year());
            result.startDate.setMonth(day.month());
            result.startDate.set_mDay(day.day());
            result.startDate.set_wDay(day.day_of_week());
        }

        result.startDate.setHour(0);
        result.startDate.setMinute(0);

        result.setTag(utils::ENUSHolidaysParser);
        return result;
    }
};

#undef PATTERN

#endif