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
	, m_curve_calculated(calculateCurve(m_curve))
	, m_curve_interpolated(interpolateCurve(m_curve_calculated))
{
	//m_curve_calculated = mkt->get_yield_curve(curve_name.substr(ir_curve_discount_prefix.length(), 3));
}


std::map<int, double> CurveDiscount::interpolateCurve(std::map<int, double> curve_caluclated) // TODO: by reference
{
	std::map<int, double> interpolated_curve;
	std::map<int, double>::iterator iter;

	for ( iter = std::next(curve_caluclated.begin()); iter != curve_caluclated.end(); iter++ )
	{
		double this_value = iter->second;
		double prev_value = std::prev(iter)->second;
		int this_day = iter->first;
		int prev_day = std::prev(iter)->first;
		double test = (this_day - prev_day) / 365.0;
		interpolated_curve[iter->first] = ( this_value - prev_value )/ test; // r(i,i+1)(T(i+1)-T(i))/(T(i+1)-T(i))
		std::cout << iter->first  << "," << (this_value - prev_value) << "," << ((this_day - prev_day) / 365.0) << std::endl;
		std::cout << (this_value - prev_value) / test << std::endl;
	}
	return interpolated_curve;
}

std::map<int, double> CurveDiscount::calculateCurve(std::map<string, double> original_curve ) // T(i):r(i)T(i)
{	
	/*double date_interval(10);
	Date new_date(today.m_serial() + 30)*/

	std::map<int, double> calculated_curve; //
	for (auto const& tenor_value : original_curve)
	{
		const std::string tenor = tenor_value.first.substr(3, tenor_value.first.length() - 7); //"10Y"
		char period = tenor[tenor.size() - 1];
		unsigned int day_count = 0;
		switch (period) {
			case 'D': {
				day_count = 1;
				break;
			}
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
		unsigned int n = atoi(tenor.substr(0, tenor.size()-1).c_str()); //TODO: confirm the reason why this works
		//Date tenor_date(today.m_serial() + n*day_count);
		//double dt = time_frac(m_today, tenor_date);
		int total_period = n * day_count;
		double dt = total_period / 365.0;
		calculated_curve[total_period] = dt * tenor_value.second;
		std::cout << total_period << "," << dt * tenor_value.second << std::endl;
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
	int days_from_today = t.m_serial() - m_today.m_serial();
	MYASSERT((!(days_from_today > std::prev(m_curve_calculated.end())->first)), "cannot get discount factor for date beyond the longest tenor: " << t);
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

	int prev_day = 0;
	double prev_value = 0;
	double interpolated_rate = 0;

	std::map<int, double>::const_iterator iter; // TODO: confirm this is correct
	for ( iter = m_curve_calculated.begin(); iter != m_curve_calculated.end(); iter++ )
	{
		if (days_from_today < iter->first) // TODO; how about day t is smaller than the 
		{
			prev_value = std::prev(iter)->second;
			prev_day = std::prev(iter)->first;
			break;
		}
	}

	for (iter = m_curve_interpolated.begin(); iter != m_curve_calculated.end(); iter++)
	{
		if (days_from_today < iter->first) // TODO; how about day t is smaller than the 
		{
			interpolated_rate = iter->second;
			break;
		}
			
	}
	double test = (days_from_today - prev_day)/365.0;
	std::cout << test << std::endl;
	double df = std::exp(-prev_value-interpolated_rate*(test));
																										  // 3.1 r(0,i), r(0,i+1) ---> ri,i+1
	// 3.2 df
    double dt = time_frac(m_today, t);
	std::cout << dt << std::endl;
	return df;
}

} // namespace minirisk
