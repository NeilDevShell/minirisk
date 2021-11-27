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

    friend long operator-(const Date& d1, const Date& d2);

    static const std::array<unsigned, 12> days_in_month;  // num of days in month M in a normal year
    static const std::array<unsigned, 12> days_ytd;      // num of days since 1-jan to 1-M in a normal year
	static const std::array<unsigned, 12> days_ytd_leap; // num of days since 1-jan to 1-M in a leap year
    static const std::array<unsigned, n_years> days_epoch;   // num of days since 1-jan-1900 to 1-jan-yyyy (until 2200)

public:
    // Default constructor
    // Date() : m_y(1970), m_m(1), m_d(1), m_is_leap(false) {}
    Date() : m_serial(0) {}
    // Constructor where the input value is checked.
    Date(unsigned year, unsigned month, unsigned day)
    {
        init(year, month, day);
    }

    Date(unsigned i_serial)
    {
        m_serial = i_serial;
    }

    void init(unsigned year, unsigned month, unsigned day)
    {
        check_valid(year, month, day);
        bool m_is_leap = is_leap_year(year);
        m_serial = days_epoch[year - 1900] + days_ytd[month - 1] + ((month > 2 && m_is_leap) ? 1 : 0) + (day - 1);
    }

    static void check_valid(unsigned y, unsigned m, unsigned d);

    bool operator<(const Date& d) const
    {
        return m_serial < d.serial();
    }

    bool operator==(const Date& d) const
    {
        return m_serial == d.serial();
    }

    bool operator>(const Date& d) const
    {
        return m_serial > d.serial();
    }

    static bool is_leap_year(unsigned yr);
    unsigned long serial() const
    {
        return m_serial;
    }
    // In YYYYMMDD format
    std::string to_string(bool pretty = true) const
    {

        unsigned long t_serial = m_serial; 
		unsigned long m_d = 0;
		unsigned short m_m = 0;

		auto iter_year = std::upper_bound(days_epoch.begin(), days_epoch.end(), m_serial);
		//std::cout << *std::prev(iter_year) << std::endl;
		t_serial -= *std::prev(iter_year);
		unsigned short m_y = first_year + (unsigned short)std::distance(days_epoch.begin(), iter_year) - 1;
		//std::cout << m_y << std::endl;
		std::array<unsigned, 12>::const_iterator iter_month;
		// leap year and any date after Feb 29 in leap year 
		if (t_serial < 59 || !is_leap_year(m_y))
		{
			iter_month = std::upper_bound(days_ytd.begin(), days_ytd.end(), t_serial);
			m_m = (unsigned short)std::distance(days_ytd.begin(), iter_month);
		}
		// Not leap year or any date before Feb 29 in leap year 
		else
		{
			iter_month = std::upper_bound(days_ytd_leap.begin(), days_ytd_leap.end(), t_serial);
			m_m = (unsigned short)std::distance(days_ytd_leap.begin(), iter_month);
		}
		t_serial -= *std::prev(iter_month);
		/*std::cout << m_m << std::endl;
		std::cout << t_serial << std::endl;*/

        m_d = t_serial + 1;
        return pretty
            ? std::to_string((int)m_d) + "-" + std::to_string((int)m_m) + "-" + std::to_string(m_y)
            : std::to_string(m_y) + padding_dates((int)m_m) + padding_dates((int)m_d);
    }

private:
    unsigned long m_serial;
};

long operator-(const Date& d1, const Date& d2);

inline double time_frac(const Date& d1, const Date& d2)
{
    return static_cast<double>(d2 - d1) / 365.0;
}

} // namespace minirisk
