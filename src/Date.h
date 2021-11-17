#pragma once

#include "Macros.h"
#include <string>
#include <array>

namespace minirisk {

struct Date
{
public:
    static const unsigned first_year = 1900;
    static const unsigned last_year = 2200;
    static const unsigned n_years = last_year - first_year;

private:
    static std::string padding_dates(unsigned);

    // number of days elapsed from beginning of the year
    unsigned day_of_year() const;

    friend long operator-(const Date& d1, const Date& d2);

    static const std::array<unsigned, 12> days_in_month;  // num of days in month M in a normal year
    static const std::array<unsigned, 12> days_ytd;      // num of days since 1-jan to 1-M in a normal year
    static const std::array<unsigned, n_years> days_epoch;   // num of days since 1-jan-1900 to 1-jan-yyyy (until 2200)

public:
    // Default constructor
    // Date() : m_y(1970), m_m(1), m_d(1), m_is_leap(false) {}
    Date() : serial(0) {}
    // Constructor where the input value is checked.
    Date(unsigned year, unsigned month, unsigned day)
    {
        init(year, month, day);
    }

    Date(unsigned i_serial)
    {
        serial = i_serial;
    }

    void init(unsigned year, unsigned month, unsigned day)
    {
        check_valid(year, month, day);
        bool m_is_leap = is_leap_year(year);
        serial = days_epoch[year - 1900] + days_ytd[month - 1] + ((month > 2 && m_is_leap) ? 1 : 0) + (day - 1);
    }

    static void check_valid(unsigned y, unsigned m, unsigned d);

    bool operator<(const Date& d) const
    {
        return serial < d.serial;
    }

    bool operator==(const Date& d) const
    {
        return serial == d.serial;
    }

    bool operator>(const Date& d) const
    {
        return d < (*this);
    }

    static bool is_leap_year(unsigned yr);
    unsigned long m_serial() const
    {
        return serial;
    }
    // In YYYYMMDD format
    std::string to_string(bool pretty = true) const
    {
        unsigned long m_y = first_year;
        unsigned long m_m = 12;
        unsigned long m_d;
        unsigned long t_serial = serial; 
        bool is_leap_month = false;
        while(t_serial >= 365)
        {
            ++m_y;
            t_serial -= 365;
            if (Date::is_leap_year(m_y))
            {
                if(t_serial == 59)
                {
                    is_leap_month = true;
                }
                if(t_serial > 59)
                {
                    --t_serial;
                }
            }
        }

        while (t_serial < days_ytd[m_m - 1])
        {
            --m_m;
            if (m_m == 3 && Date::is_leap_year(m_y) && is_leap_month)
            {
                --m_m;
                break;
                t_serial += days_ytd[m_m - 1];
            }
        }
        t_serial -= days_ytd[m_m - 1];
        
        m_d = t_serial + 1;
        return pretty
            ? std::to_string((int)m_d) + "-" + std::to_string((int)m_m) + "-" + std::to_string(m_y)
            : std::to_string(m_y) + padding_dates((int)m_m) + padding_dates((int)m_d);
    }

private:
    unsigned long serial;
};

long operator-(const Date& d1, const Date& d2);

inline double time_frac(const Date& d1, const Date& d2)
{
    return static_cast<double>(d2 - d1) / 365.0;
}

} // namespace minirisk
