#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

CurveDiscount::CurveDiscount(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today)
    , m_name(curve_name)
	, m_curve(mkt->get_yield_curve(curve_name.substr(ir_curve_discount_prefix.length(), 3))) // Original curve
	, m_curve_calculated(calculateCurve(m_curve)) // Calculated curve - r(i)T(i)
	, m_curve_interpolated(interpolateCurve(m_curve_calculated)) // Interpolated curve - r(i,i+1)(T(i+1)-T(i))/(T(i+1)-T(i))
{
}

std::map<int, double> CurveDiscount::interpolateCurve(const std::map<int, double>& curve_caluclated) // TODO: by reference
{
	std::map<int, double> interpolated_curve;
	std::map<int, double>::const_iterator iter;

	for ( iter = std::next(curve_caluclated.begin()); iter != curve_caluclated.end(); iter++ )
	{
		double this_value = iter->second;
		double prev_value = std::prev(iter)->second;
		int this_day = iter->first;
		int prev_day = std::prev(iter)->first;
		interpolated_curve[iter->first] = ( this_value - prev_value )/ (((double)this_day - (double)prev_day) / 365.0); // r(i,i+1)(T(i+1)-T(i))/(T(i+1)-T(i))
	}
	return interpolated_curve;
}

std::map<int, double> CurveDiscount::calculateCurve(const std::map<string, double>& original_curve ) // T(i):r(i)T(i)
{
	std::map<int, double> calculated_curve;
	calculated_curve[0] = 0;
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
		int total_period = n * day_count;
		double dt = total_period / 365.0;
		calculated_curve[total_period] = dt * tenor_value.second;
	}
	return calculated_curve;
}

double  CurveDiscount::df(const Date& t) const
{
    MYASSERT((!(t < m_today)), "Curve " <<  m_name  << ", DF not available before anchor date " << m_today << ", requested "  << t);
	// TODO Some of the below functions may be better in Constructor to improve the performance
	// 1. get the longest tenor and convert to date; throw if t is larger than the farthest date
	const int days_from_today = (int)t.serial() - (int)m_today.serial();
	Date last_tenor((int)m_today.serial() + std::prev(m_curve_calculated.end())->first);
	MYASSERT((!(days_from_today > std::prev(m_curve_calculated.end())->first)), "Curve " << m_name << ", DF not available beyond last tenor date " << last_tenor << ", requested " << t);
	// 2. based on t, get the closet i and i+1 given all tenors and their corresponding curve (all tenors should be saved into CurveDiscount already)
	// 3. do interploation and return discount factor
	// 3.1 r(0,i), r(0,i+1) ---> ri,i+1
	int prev_day = 0;
	double prev_value = 0;
	double interpolated_rate = 0;
	auto up_calculated = m_curve_calculated.upper_bound(days_from_today);
	prev_value = std::prev(up_calculated)->second;
	prev_day = std::prev(up_calculated)->first;

	interpolated_rate = m_curve_interpolated.find(up_calculated->first)->second;
	double df = std::exp(-prev_value-interpolated_rate*(((double)days_from_today - (double)prev_day) / 365.0));
	// 3.2 df
    //double dt = time_frac(m_today, t);
	//std::cout << dt << std::endl;
	return df;
}

} // namespace minirisk
