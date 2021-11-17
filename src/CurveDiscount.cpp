#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

CurveDiscount::CurveDiscount(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today)
    , m_name(curve_name)
    //, m_rate(mkt->get_yield(curve_name.substr(ir_curve_discount_prefix.length(),3)))  // TODO: need to have the full list matching regex instead of one number.
	, m_curve(mkt->get_yield_curve(curve_name.substr(ir_curve_discount_prefix.length(), 3)))
	, m_curve_calculated(calculateCurve(m_curve, m_today))
{
	//m_curve_calculated = mkt->get_yield_curve(curve_name.substr(ir_curve_discount_prefix.length(), 3));
}
std::map<Date, double> CurveDiscount::calculateCurve(std::map<string, double> original_curve, const Date& today)
{	
	/*double date_interval(10);
	Date new_date(today.m_serial() + 30)*/

	std::map<Date, double> calculated_curve;
	for (auto const& tenor_value : original_curve)
	{
		char period = tenor_value.first[tenor_value.first.size() - 1];
		unsigned day_count = 0;
		switch (period) {
			case 'W': {
				day_count = 7;
				break;
			}
			case 'M':
			{
				day_count = 30;
				break;
			}
			case 'Y':
			{
				day_count = 365;
				break;
			}
			default:
				MYASSERT(true, "Unrecongnized period unit " << period);
		}
		unsigned n = atoi(tenor_value.first.c_str());
		Date tenor_date(today.m_serial() + n*day_count);
		double dt = time_frac(m_today, tenor_date);
		calculated_curve[tenor_date] = dt * tenor_value.second;
	}
	return calculated_curve;
}

bool comp(Date a, Date b)
{
	return(a.m_serial() > b.m_serial());
}

double  CurveDiscount::df(const Date& t) const
{
    MYASSERT((!(t < m_today)), "cannot get discount factor for date in the past: " << t);
	// TODO Some of the below functions may be better in Constructor to improve the performance
	// 1. get the longest tenor and convert to date; throw if t is larger than the farthest date
	MYASSERT((!(t > std::prev(m_curve_calculated.end())->first)), "cannot get discount factor for date in the past: " << t);
	// 2. based on t, get the closet i and i+1 given all tenors and their corresponding curve (all tenors should be saved into CurveDiscount already)
	// 3. do interploation and return discount factor
	
	//auto low = std::lower_bound(m_curve_calculated.begin(), m_curve_calculated.end(), t ); //          ^
	//auto up = std::upper_bound(m_curve_calculated.begin(), m_curve_calculated.end(), t ); //    
	//Date test = low->first;
	/*std::map<Date, double>::iterator it;
	for (it = m_curve_calculated.begin(); it != m_curve_calculated.end(); it++)
	{
		if (t.m_serial() > it->first.m_serial() && t.m_serial() < std::next(it)->first.m_serial())
			break;
	}*/

	unsigned long prev = 0;
	unsigned long next = 0;
	double prev_value = 0;
	double next_value = 0;

	for (auto const& tenor_value : m_curve_calculated)
	{
		if (t.m_serial() > tenor_value.first.m_serial())
		{
			prev = tenor_value.first.m_serial();
			prev_value = tenor_value.second;
		}
		else //TODO: how about the same?
		{
			next = tenor_value.first.m_serial();
			next_value = tenor_value.second;
			break;
		}
	}
	double interpolated_rate = (-prev_value + next_value)/(next-prev);
																										  // 3.1 r(0,i), r(0,i+1) ---> ri,i+1
	// 3.2 df
    double dt = time_frac(m_today, t);
	return std::exp(-prev_value-interpolated_rate*dt);
}

} // namespace minirisk
